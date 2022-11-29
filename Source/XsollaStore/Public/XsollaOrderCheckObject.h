// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XsollaStoreDataModel.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "XsollaUtilsHttpRequestHelper.h"
#include "XsollaOrderCheckObject.generated.h"

class IWebSocket;

DECLARE_DELEGATE_OneParam(FOnOrderCheckSuccess, int32);
DECLARE_DELEGATE_ThreeParams(FOnOrderCheckError, int32, int32, const FString&);
DECLARE_DELEGATE_ThreeParams(FOnOrderCheck, int32, EXsollaOrderStatus, FXsollaOrderContent);

UCLASS(BlueprintType)
class UXsollaOrderCheckObject : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(const FString& InAccessToken, const int32 InOrderId,
		const FOnOrderCheckSuccess& InOnSuccess, const FOnOrderCheckError& InOnError, int32 InWebSocketLifeTime = 300, int32 InShortPollingLifeTime = 600);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Connect();

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Destroy();

private:
	TSharedPtr<IWebSocket> Websocket;

	FOnOrderCheckSuccess OnSuccess;

	FOnOrderCheckError OnError;

	FString AccessToken;

	int32 OrderId;

	int32 WebSocketLifeTime;

	int32 ShortPollingLifeTime;

	FTimerHandle WebSocketTimerHandle;

	FTimerHandle ShortPollingTimerHandle;

	bool bShortPollingExpired = false;

	void OnConnected();

	void OnConnectionError(const FString& Error);

	void OnMessage(const FString& Message);

	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	
	void OnWebSocketExpired();

	void OnShortPollingExpired();

	void ActivateShortPolling();

	void ShortPollingCheckOrder();

	void CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnOrderCheck SuccessCallback, FOnOrderCheckError ErrorCallback);
};