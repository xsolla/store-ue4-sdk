// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaStoreDataModel.h"

#include "XsollaWebsocket.generated.h"

class IWebSocket;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCheckOrderViaWebsocket, int32, OrderId, EXsollaOrderStatus, OrderStatus);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWebsocketError, const FString&, ErrorMessage);

UCLASS(BlueprintType)
class UXsollaWebsocket : public UObject
{
	GENERATED_BODY()
public:

	TSharedPtr<IWebSocket> Websocket;

	FOnCheckOrderViaWebsocket OnStatusReceived;

	FOnWebsocketError OnError;

	int32 LifeTime;

	FTimerHandle TimerHandle;
	
	void Init(const FString& Url, const FString& Protocol, int32 SocketLifeTime = 300);

	void OnExpired();
	
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Websocket")
	void Connect();

	void Destroy();

	void OnConnected();

	void OnConnectionError(const FString& Error);
	
	void OnMessage(const FString& Message);

	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
};