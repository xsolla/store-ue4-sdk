// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaOrderCheckObject.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "TimerManager.h"
#include "XsollaStoreDataModel.h"
#include "XsollaUtilsUrlBuilder.h"
#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"
#include "XsollaUtilsHttpRequestHelper.h"
#include "XsollaStoreDefines.h"
#include "CentrifugoServiceSubsystem.h"
#include "Engine/GameInstance.h"

void UXsollaOrderCheckObject::Init(const FString& InAccessToken, const int32 InOrderId, bool bShouldStartWithCentrifugo, const FOnOrderCheckSuccess& InOnSuccess, const FOnOrderCheckError& InOnError, int32 InShortPollingLifeTime)
{
	AccessToken = InAccessToken;
	OrderId = InOrderId;
	ShortPollingLifeTime = FMath::Clamp(InShortPollingLifeTime, 1, 3600);

	OnSuccess = InOnSuccess;
	OnError = InOnError;

	if (bShouldStartWithCentrifugo)
	{
		StartCentrifugoTracking();
	}
	else
	{
		ActivateShortPolling();
	}
}

void UXsollaOrderCheckObject::Destroy()
{
	StopCentrifugoTracking();
	bShortPollingExpired = true;
	GetWorld()->GetTimerManager().ClearTimer(ShortPollingTimerHandle);

	UE_LOG(LogXsollaStore, Log, TEXT("Destroy XsollaOrderCheckObject."));
}

const FString& UXsollaOrderCheckObject::GetAccessToken() const
{
	return AccessToken;
}

void UXsollaOrderCheckObject::OnConnectionError()
{
	ActivateShortPolling();
}

void UXsollaOrderCheckObject::OnOrderStatusUpdated(const FOrderStatusData Data)
{
	if (Data.status.IsEmpty())
	{
		return;
	}

	if (Data.order_id != OrderId)
	{
		return;
	}

	EXsollaOrderStatus OrderStatus = EXsollaOrderStatus::Unknown;

	if (Data.status == TEXT("new"))
	{
		OrderStatus = EXsollaOrderStatus::New;
	}
	else if (Data.status == TEXT("paid"))
	{
		OrderStatus = EXsollaOrderStatus::Paid;
	}
	else if (Data.status == TEXT("done"))
	{
		OrderStatus = EXsollaOrderStatus::Done;
	}
	else if (Data.status == TEXT("canceled"))
	{
		OrderStatus = EXsollaOrderStatus::Canceled;
	}
	else
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: Centrifugo: Unknown order status: %s [%d]"), *VA_FUNC_LINE, *Data.status, Data.order_id);
	}

	if (OrderStatus == EXsollaOrderStatus::Done)
	{
		OnSuccess.ExecuteIfBound(Data.order_id);
	}
	if (OrderStatus == EXsollaOrderStatus::Canceled)
	{
		OnError.ExecuteIfBound(0, 0, TEXT("Order canceled."));
	}
}

void UXsollaOrderCheckObject::OnClosed()
{
	ActivateShortPolling();
}

void UXsollaOrderCheckObject::OnShortPollingExpired()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Short polling expired."));
	bShortPollingExpired = true;
}

void UXsollaOrderCheckObject::StartCentrifugoTracking()
{
	UE_LOG(LogXsollaStore, Log, TEXT("StartCentrifugoTracking"));
	UCentrifugoServiceSubsystem* CentrifugoServiceSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCentrifugoServiceSubsystem>();
	CentrifugoServiceSubsystem->AddTracker(this);
	CentrifugoServiceSubsystem->OrderStatusUpdated.AddUObject(this, &UXsollaOrderCheckObject::OnOrderStatusUpdated);
	CentrifugoServiceSubsystem->Error.AddUObject(this, &UXsollaOrderCheckObject::OnConnectionError);
	CentrifugoServiceSubsystem->Close.AddUObject(this, &UXsollaOrderCheckObject::OnClosed);
}

void UXsollaOrderCheckObject::StopCentrifugoTracking()
{
	UE_LOG(LogXsollaStore, Log, TEXT("StopCentrifugoTracking"));
	UCentrifugoServiceSubsystem* CentrifugoServiceSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCentrifugoServiceSubsystem>();
	CentrifugoServiceSubsystem->RemoveTracker(this);
	CentrifugoServiceSubsystem->OrderStatusUpdated.RemoveAll(this);
	CentrifugoServiceSubsystem->Error.RemoveAll(this);
	CentrifugoServiceSubsystem->Close.RemoveAll(this);
}

void UXsollaOrderCheckObject::ActivateShortPolling()
{
	StopCentrifugoTracking();
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
	CheckOrderSuccessCallback.BindLambda([this](int32 InOrderId, EXsollaOrderStatus InOrderStatus, FXsollaOrderContent InOrderContent)
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = XsollaUtilsHttpRequestHelper::CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AccessToken, FString(), TEXT("STORE"), XSOLLA_STORE_VERSION);
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
