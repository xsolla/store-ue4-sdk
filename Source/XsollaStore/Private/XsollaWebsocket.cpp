// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaWebsocket.h"

#include "WebSocketsModule.h"
#include "XsollaStoreDataModel.h"

void UXsollaWebsocket::Init(const FString& Url, const FString& Protocol, int32 SocketLifeTime)
{

	LifeTime = FMath::Clamp(SocketLifeTime, 1, 3600);
	
	Websocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocol);

	Websocket->OnConnected().AddUObject(this, &UXsollaWebsocket::OnConnected);
	Websocket->OnConnectionError().AddUObject(this, &UXsollaWebsocket::OnConnectionError);
	Websocket->OnMessage().AddUObject(this, &UXsollaWebsocket::OnMessage);
	Websocket->OnClosed().AddUObject(this, &UXsollaWebsocket::OnClosed);
}

void UXsollaWebsocket::OnExpired()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Websocket object expired."));
	OnError.ExecuteIfBound(TEXT("Timeout"));
}

void UXsollaWebsocket::Connect()
{

	if (!Websocket.IsValid())
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("Can't connect. Websocket is invalid."));
		return;
	}

	Websocket->Connect();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UXsollaWebsocket::OnExpired, LifeTime, false);
}

void UXsollaWebsocket::Destroy()
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

void UXsollaWebsocket::OnConnected()
{
	UE_LOG(LogXsollaStore, Log, TEXT("Connected to websocket server."));
}

void UXsollaWebsocket::OnConnectionError(const FString& Error)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Failed to connect to websocket server with error: \"%s\"."), *Error);
	
	OnError.ExecuteIfBound(FString::Printf(TEXT("Failed to connect to websocket server with error: \"%s\"."), *Error));
}

void UXsollaWebsocket::OnMessage(const FString& Message)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Received message from websocket server: \"%s\"."), *Message);

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

void UXsollaWebsocket::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogXsollaStore, Log, TEXT("Connection to websocket server has been closed with status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
	OnError.ExecuteIfBound(TEXT("Connection to websocket server has been closed."));
}