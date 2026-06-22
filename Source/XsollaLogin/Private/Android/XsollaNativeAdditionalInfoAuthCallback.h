// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaLoginSubsystem.h"
#include <atomic>

#include "XsollaNativeAdditionalInfoAuthCallback.generated.h"

class UXsollaLoginSubsystem;

UCLASS()
class XSOLLALOGIN_API UXsollaNativeAdditionalInfoAuthCallback : public UObject
{
	GENERATED_BODY()

public:
	void BindSuccessDelegate(const FOnAuthUpdate& OnSuccess, UXsollaLoginSubsystem* InLoginSubsystem);
	void BindErrorDelegate(const FOnAuthError& OnError);

	void ExecuteSuccess(const FString& AuthenticationCode, const FString& AuthenticationToken);
	void ExecuteCancel();
	void ExecuteError(const FString& ErrorCode, const FString& ErrorMessage);

private:
	UPROPERTY()
	FOnAuthUpdate OnAuthSuccessDelegate;
	UPROPERTY()
	FOnAuthError OnAuthErrorDelegate;
	UPROPERTY()
	UXsollaLoginSubsystem* LoginSubsystem;
	std::atomic<bool> bTerminalDispatched{false};
};
