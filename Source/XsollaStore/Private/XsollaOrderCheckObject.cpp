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

void UXsollaOrderCheckObject::Init(const FString& Url, const FString& Protocol,
	const FOnOrderCheckSuccess& InOnStatusReceived, const FOnOrderCheckError& InOnError,
	const FOnOrderCheckTimeout& InOnTimeout, int32 SocketLifeTime)
{

	LifeTime = FMath::Clamp(SocketLifeTime, 1, 3600);

	OnStatusReceived = InOnStatusReceived;
	OnError = InOnError;
	OnTimeout = InOnTimeout;

	Websocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocol);
	Websocket->OnConnected().AddUObject(this, &UXsollaOrderCheckObject::OnConnected);
	Websocket->OnConnectionError().AddUObject(this, &UXsollaOrderCheckObject::OnConnectionError);
	Websocket->OnMessage().AddUObject(this, &UXsollaOrderCheckObject::OnMessage);
	Websocket->OnClosed().AddUObject(this, &UXsollaOrderCheckObject::OnClosed);
}

void UXsollaOrderCheckObject::Connect()
{

	if (!Websocket.IsValid())
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("Can't connect. Websocket invalid."));
		return;
	}

	Websocket->Connect();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UXsollaOrderCheckObject::OnExpired, LifeTime, false);
}

void UXsollaOrderCheckObject::Destroy()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Destroy websocket."));
	if (Websocket.IsValid())
	{
		Websocket->OnConnected().RemoveAll(this);
		Websocket->OnConnectionError().RemoveAll(this);
		Websocket->OnClosed().RemoveAll(this);
		Websocket->Close();
		Websocket = nullptr;
	}

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UXsollaOrderCheckObject::OnConnected()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Connected to the websocket server."));
}

void UXsollaOrderCheckObject::OnConnectionError(const FString& Error)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Failed to connect to a websocket server with an error: \"%s\"."), *Error);
	OnError.ExecuteIfBound(FString::Printf(TEXT("Failed to connect to a websocket server with an error: \"%s\"."), *Error));
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
	auto OrderId = JsonObject->GetIntegerField(TEXT("order_id"));

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
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: WebsocketObject: Unknown order status: %s [%d]"), *VA_FUNC_LINE, *OrderStatusStr, OrderId);
	}

	OnStatusReceived.ExecuteIfBound(OrderId, OrderStatus);
}

void UXsollaOrderCheckObject::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Connection to the websocket server has been closed with the status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
	OnError.ExecuteIfBound(TEXT("Connection to the websocket server has been closed."));
}

void UXsollaOrderCheckObject::OnExpired()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Websocket object expired."));
	OnTimeout.ExecuteIfBound();
}
