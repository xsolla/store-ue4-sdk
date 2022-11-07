// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XsollaStoreDataModel.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "XsollaStoreDefines.h"
#include "XsollaOrderCheckObject.generated.h"

class IWebSocket;

DECLARE_DELEGATE_TwoParams(FOnOrderCheckSuccess, int32, EXsollaOrderStatus);
DECLARE_DELEGATE_OneParam(FOnOrderCheckError, const FString&);
DECLARE_DELEGATE(FOnOrderCheckTimeout);

UCLASS(BlueprintType)
class UXsollaOrderCheckObject : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(const FString& Url, const FString& Protocol,
		const FOnOrderCheckSuccess& InOnStatusReceived, const FOnOrderCheckError& InOnError,
		const FOnOrderCheckTimeout& InOnTimeout, int32 SocketLifeTime = 300);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Connect();

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Destroy();

private:
	TSharedPtr<IWebSocket> Websocket;

	FOnOrderCheckSuccess OnStatusReceived;

	FOnOrderCheckError OnError;

	FOnOrderCheckTimeout OnTimeout;

	int32 LifeTime;

	FTimerHandle TimerHandle;

	void OnConnected();

	void OnConnectionError(const FString& Error);

	void OnMessage(const FString& Message);

	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	
	void OnExpired();
};