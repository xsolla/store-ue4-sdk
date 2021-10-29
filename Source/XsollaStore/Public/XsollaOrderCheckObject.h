// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XsollaStoreDataModel.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "XsollaOrderCheckObject.generated.h"

class IWebSocket;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnOrderCheckViaWebsocket, int32, OrderId, EXsollaOrderStatus, OrderStatus);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWebsocketError, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE(FOnWebsocketTimeout);

UCLASS(BlueprintType)
class UXsollaOrderCheckObject : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(const FString& Url, const FString& Protocol,
		const FOnOrderCheckViaWebsocket& InOnStatusReceived, const FOnWebsocketError& InOnError,
		const FOnWebsocketTimeout& InOnTimeout, int32 SocketLifeTime = 300);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Connect();

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Destroy();

private:
	TSharedPtr<IWebSocket> Websocket;

	FOnOrderCheckViaWebsocket OnStatusReceived;

	FOnWebsocketError OnError;

	FOnWebsocketTimeout OnTimeout;

	int32 LifeTime;

	FTimerHandle TimerHandle;

	void OnConnected();

	void OnConnectionError(const FString& Error);

	void OnMessage(const FString& Message);

	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	
	void OnExpired();
};