// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "XsollaCentrifugoDataModel.h"
#include "Engine/EngineTypes.h"
#include "CentrifugoServiceSubsystem.generated.h"

class UXsollaOrderCheckObject;
class UCentrifugoClient;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOrderStatusUpdated, const FOrderStatusData);

UCLASS(NotBlueprintType)
class UCentrifugoServiceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// End USubsystem

	void AddTracker(const UXsollaOrderCheckObject* Tracker);
	void RemoveTracker(const UXsollaOrderCheckObject* Tracker);

	FOnOrderStatusUpdated OrderStatusUpdated;
	FSimpleMulticastDelegate Error;
	FSimpleMulticastDelegate Close;

protected:
	/** Cached Xsolla Store project id */
	FString ProjectID;

private:
	void CreateCentrifugoClient(const FString& AccessToken);
	void TerminateCentrifugoClient();

	void OnCentrifugoMessageReceived(const FString& Message);
	void OnCentrifugoError(const FString& ErrorMessage);
	void OnCentrifugoClosed(const FString& Reason);

	void DoPing();

	int32 PingInterval = 25;
	int32 TimeoutLimit = 600;
	int32 PingCounter = 0;
	int32 TimeoutCounter = 0;
	FTimerHandle PingTimerHandle;

	UPROPERTY()
	UCentrifugoClient* CentrifugoClient;

	UPROPERTY()
	TArray<const UXsollaOrderCheckObject*> Trackers;

};
