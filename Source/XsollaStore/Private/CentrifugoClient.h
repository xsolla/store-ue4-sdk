// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IWebSocket.h"
#include "CentrifugoClient.generated.h"

DECLARE_DELEGATE_OneParam(FOnCentrifugoMessageReceived, const FString&);
DECLARE_DELEGATE_OneParam(FOnCentrifugoError, const FString&);
DECLARE_DELEGATE_OneParam(FOnCentrifugoClosed, const FString&);

UCLASS()
class UCentrifugoClient : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	FSimpleDelegate Opened;
	FOnCentrifugoMessageReceived MessageReceived;
	FOnCentrifugoError Error;
	FOnCentrifugoClosed Closed;

	void Connect();
	void Disconnect();
	void Send(const FString& Data);
	void SendPing();
	bool IsAlive();

private:
	TSharedPtr<IWebSocket> WebSocket;
	static FString PingMessage;
	static FString PongMessage;

	void OnSocketConnected();
	void OnSocketMessage(const FString& Message);
	void OnSocketConnectionError(const FString& ErrorMessage);
	void OnSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
};
