// Copyright 2023 Xsolla Inc. All Rights Reserved.

#include "CentrifugoClient.h"
#include "WebSocketsModule.h"
#include "XsollaStoreDefines.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

FString UCentrifugoClient::PingMessage = TEXT("{}");
FString UCentrifugoClient::PongMessage = TEXT("{}");

UCentrifugoClient::UCentrifugoClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(TEXT("wss://ws-store.xsolla.com/connection/websocket"), TEXT("wss"));
}

void UCentrifugoClient::Connect()
{
	WebSocket->OnConnected().AddUObject(this, &UCentrifugoClient::OnSocketConnected);
	WebSocket->OnMessage().AddUObject(this, &UCentrifugoClient::OnSocketMessage);
	WebSocket->OnConnectionError().AddUObject(this, &UCentrifugoClient::OnSocketConnectionError);
	WebSocket->OnClosed().AddUObject(this, &UCentrifugoClient::OnSocketClosed);
	WebSocket->Connect();
}

void UCentrifugoClient::Disconnect()
{
	WebSocket->OnConnected().RemoveAll(this);
	WebSocket->OnMessage().RemoveAll(this);
	WebSocket->OnConnectionError().RemoveAll(this);
	WebSocket->OnClosed().RemoveAll(this);
	WebSocket->Close();
	WebSocket = nullptr;
}

void UCentrifugoClient::Send(const FString& Data)
{
	FString CleanData = Data;
	CleanData = CleanData.Replace(TEXT("\n"), TEXT(""));
	CleanData = CleanData.Replace(TEXT("\r"), TEXT(""));
	CleanData.RemoveSpacesInline();

	UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo. Websocket send data: %s"), *CleanData);
	WebSocket->Send(CleanData);
}

bool UCentrifugoClient::IsAlive()
{
	return WebSocket->IsConnected();
}

void UCentrifugoClient::OnSocketConnected()
{
	UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo. Connected to the websocket server."));
	Opened.ExecuteIfBound();
}

void UCentrifugoClient::OnSocketMessage(const FString& Message)
{
	if (Message == PingMessage)
	{
		UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo. Websocket handshake."));
		WebSocket->Send(PongMessage);
		return;
	}

	UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo. Received message from the websocket server: \"%s\"."), *Message);
	MessageReceived.ExecuteIfBound(Message);
}

void UCentrifugoClient::OnSocketConnectionError(const FString& ErrorMessage)
{
	UE_LOG(LogXsollaCentrifugo, Warning, TEXT("Centrifugo. Failed to connect to a websocket server with an error: \"%s\"."), *ErrorMessage);
	Error.ExecuteIfBound(ErrorMessage);
}

void UCentrifugoClient::OnSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo. Connection to the websocket server has been closed with the status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
	Closed.ExecuteIfBound(Reason);
}