// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XsollaStoreDataModel.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "XsollaUtilsHttpRequestHelper.h"
#include "XsollaOrderCheckObject.generated.h"

struct FOrderStatusData;

DECLARE_DELEGATE_OneParam(FOnOrderCheckSuccess, int32);
DECLARE_DELEGATE_ThreeParams(FOnOrderCheckError, int32, int32, const FString&);
DECLARE_DELEGATE_ThreeParams(FOnOrderCheck, int32, EXsollaOrderStatus, FXsollaOrderContent);

UCLASS(BlueprintType)
class UXsollaOrderCheckObject : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(const FString& InAuthToken, const int32 InOrderId, bool bShouldStartWithCentrifugo,
		const FOnOrderCheckSuccess& InOnSuccess, const FOnOrderCheckError& InOnError, int32 InShortPollingLifeTime = 600);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|OrderCheck")
	void Destroy();

private:
	FOnOrderCheckSuccess OnSuccess;

	FOnOrderCheckError OnError;

	FString AuthToken;

	int32 OrderId;

	int32 ShortPollingLifeTime;

	FTimerHandle ShortPollingTimerHandle;

	bool bShortPollingExpired = false;

	void OnConnectionError();

	void OnOrderStatusUpdated(const FOrderStatusData Data);

	void OnClosed();

	void OnShortPollingExpired();

	void StartCentrifugoTracking();

	void StopCentrifugoTracking();

	void ActivateShortPolling();

	void ShortPollingCheckOrder();

	void CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnOrderCheck SuccessCallback, FOnOrderCheckError ErrorCallback);
};