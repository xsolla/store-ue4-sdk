// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaOrderCheckObject.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "IWebSocket.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "TimerManager.h"
#include "WebSocketsModule.h"
#include "XsollaStoreDataModel.h"
#include "XsollaUtilsUrlBuilder.h"
#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"
#include "XsollaUtilsHttpRequestHelper.h"
#include "XsollaStoreDefines.h"

void UXsollaOrderCheckObject::Init(const FString& InAuthToken, const int32 InOrderId, const FOnOrderCheckSuccess& InOnSuccess, const FOnOrderCheckError& InOnError, int32 InWebSocketLifeTime, int32 InShortPollingLifeTime)
{
	AuthToken = InAuthToken;
	OrderId = InOrderId;
	WebSocketLifeTime = FMath::Clamp(InWebSocketLifeTime, 1, 3600);
	ShortPollingLifeTime = FMath::Clamp(InShortPollingLifeTime, 1, 3600);

	OnSuccess = InOnSuccess;
	OnError = InOnError;

	ActivateWebSocket();
}

void UXsollaOrderCheckObject::Destroy()
{
	DestroyWebSocket();
	bShortPollingExpired = true;
	GetWorld()->GetTimerManager().ClearTimer(ShortPollingTimerHandle);
}

void UXsollaOrderCheckObject::OnConnected()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Connected to the websocket server."));
}

void UXsollaOrderCheckObject::OnConnectionError(const FString& Error)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Failed to connect to a websocket server with an error: \"%s\"."), *Error);
	ActivateShortPolling();
}

void UXsollaOrderCheckObject::OnMessage(const FString& Message)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Received message from the websocket server: \"%s\"."), *Message);
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Message);

	FJsonSerializer::Deserialize(Reader, JsonObject);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("Can't parse received message."));
		return;
	}

	auto OrderStatusStr = JsonObject->GetStringField(TEXT("status"));
	auto ReceivedOrderId = JsonObject->GetIntegerField(TEXT("order_id"));

	EXsollaOrderStatus OrderStatus = EXsollaOrderStatus::Unknown;

	if (OrderStatusStr == TEXT("new"))
	{
		OrderStatus = EXsollaOrderStatus::New;
	}
	else if (OrderStatusStr == TEXT("paid"))
	{
		OrderStatus = EXsollaOrderStatus::Paid;
	}
	else if (OrderStatusStr == TEXT("done"))
	{
		OrderStatus = EXsollaOrderStatus::Done;
	}
	else if (OrderStatusStr == TEXT("canceled"))
	{
		OrderStatus = EXsollaOrderStatus::Canceled;
	}
	else
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: WebsocketObject: Unknown order status: %s [%d]"), *VA_FUNC_LINE, *OrderStatusStr, ReceivedOrderId);
	}

	if (OrderStatus == EXsollaOrderStatus::Done)
	{
		OnSuccess.ExecuteIfBound(ReceivedOrderId);
	}
	if (OrderStatus == EXsollaOrderStatus::Canceled)
	{
		OnError.ExecuteIfBound(0, 0, TEXT("Order canceled."));
	}
}

void UXsollaOrderCheckObject::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Connection to the websocket server has been closed with the status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
	ActivateShortPolling();
}

void UXsollaOrderCheckObject::OnWebSocketExpired()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Websocket object expired."));

	DestroyWebSocket();
	ActivateWebSocket();
}

void UXsollaOrderCheckObject::OnShortPollingExpired()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Short polling expired."));
	bShortPollingExpired = true;
}

void UXsollaOrderCheckObject::ActivateWebSocket()
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	const FString Url = XsollaUtilsUrlBuilder(TEXT("wss://store-ws.xsolla.com/sub/order/status"))
							.AddStringQueryParam(TEXT("order_id"), FString::FromInt(OrderId)) // FString casting to prevent parameters reorder.
							.AddStringQueryParam(TEXT("project_id"), Settings->ProjectID)
							.Build();

	Websocket = FWebSocketsModule::Get().CreateWebSocket(Url, TEXT("wss"));
	Websocket->OnConnected().AddUObject(this, &UXsollaOrderCheckObject::OnConnected);
	Websocket->OnConnectionError().AddUObject(this, &UXsollaOrderCheckObject::OnConnectionError);
	Websocket->OnMessage().AddUObject(this, &UXsollaOrderCheckObject::OnMessage);
	Websocket->OnClosed().AddUObject(this, &UXsollaOrderCheckObject::OnClosed);

	Websocket->Connect();
	GetWorld()->GetTimerManager().SetTimer(WebSocketTimerHandle, this, &UXsollaOrderCheckObject::OnWebSocketExpired, WebSocketLifeTime, false);
}

void UXsollaOrderCheckObject::DestroyWebSocket()
{
	GetWorld()->GetTimerManager().ClearTimer(WebSocketTimerHandle);

	UE_LOG(LogXsollaStore, Log, TEXT("Destroy XsollaOrderCheckObject."));
	if (Websocket.IsValid())
	{
		Websocket->OnConnected().RemoveAll(this);
		Websocket->OnConnectionError().RemoveAll(this);
		Websocket->OnClosed().RemoveAll(this);
		Websocket->Close();
		Websocket = nullptr;
	}
}

void UXsollaOrderCheckObject::ActivateShortPolling()
{
	UE_LOG(LogXsollaStore, Log, TEXT("ActivateShortPolling"));
	if (!GetWorld()->GetTimerManager().IsTimerActive(ShortPollingTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(ShortPollingTimerHandle, this, &UXsollaOrderCheckObject::OnShortPollingExpired, ShortPollingLifeTime, false);
		ShortPollingCheckOrder();
	}
}

void UXsollaOrderCheckObject::ShortPollingCheckOrder()
{
	UE_LOG(LogXsollaStore, Log, TEXT("ShortPollingCheckOrder"));
	FOnOrderCheck CheckOrderSuccessCallback;
	CheckOrderSuccessCallback.BindLambda([&](int32 InOrderId, EXsollaOrderStatus InOrderStatus, FXsollaOrderContent InOrderContent)
	{
		if (InOrderStatus == EXsollaOrderStatus::New || InOrderStatus == EXsollaOrderStatus::Paid)
		{
			if (bShortPollingExpired)
			{
				OnError.ExecuteIfBound(0, 0, TEXT("Short polling expired."));
			} else
			{
				FTimerHandle TimerHandle;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([&]()
				{
					ShortPollingCheckOrder();
				});
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 3.0f, false);
			}
		}

		if (InOrderStatus == EXsollaOrderStatus::Canceled)
		{
			OnError.ExecuteIfBound(0, 0, TEXT("Order canceled."));
		}
		if (InOrderStatus == EXsollaOrderStatus::Done)
		{
			OnSuccess.ExecuteIfBound(OrderId);
		}
	});

	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/order/{OrderId}"))
							.SetPathParam(TEXT("ProjectID"), Settings->ProjectID)
							.SetPathParam(TEXT("OrderId"), OrderId)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = XsollaUtilsHttpRequestHelper::CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken, FString(), TEXT("STORE"), XSOLLA_STORE_VERSION);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaOrderCheckObject::CheckOrder_HttpRequestComplete, CheckOrderSuccessCallback, OnError);
	HttpRequest->ProcessRequest();
}

void UXsollaOrderCheckObject::CheckOrder_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnOrderCheck SuccessCallback, FOnOrderCheckError ErrorCallback)
{
	FXsollaOrder Order;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaOrder::StaticStruct(), &Order, OutError))
	{
		FString OrderStatusStr = Order.status;

		EXsollaOrderStatus OrderStatus = EXsollaOrderStatus::Unknown;

		if (OrderStatusStr == TEXT("new"))
		{
			OrderStatus = EXsollaOrderStatus::New;
		}
		else if (OrderStatusStr == TEXT("paid"))
		{
			OrderStatus = EXsollaOrderStatus::Paid;
		}
		else if (OrderStatusStr == TEXT("done"))
		{
			OrderStatus = EXsollaOrderStatus::Done;
		}
		else if (OrderStatusStr == TEXT("canceled"))
		{
			OrderStatus = EXsollaOrderStatus::Canceled;
		}
		else
		{
			UE_LOG(LogXsollaStore, Warning, TEXT("%s: Unknown order status: %s [%d]"), *VA_FUNC_LINE, *OrderStatusStr, Order.order_id);
		}
		UE_LOG(LogXsollaStore, Log, TEXT("Order status received: %s %d"),  *OrderStatusStr, Order.order_id);
		SuccessCallback.ExecuteIfBound(Order.order_id, OrderStatus, Order.content);
		return;
	}

	auto ErrorMessage = OutError.errorMessage.IsEmpty() ? OutError.description : OutError.errorMessage;
	UE_LOG(LogXsollaStore, Log, TEXT("Order status error: %s"), *ErrorMessage);
	OnError.ExecuteIfBound(OutError.statusCode, OutError.errorCode, ErrorMessage);
}
