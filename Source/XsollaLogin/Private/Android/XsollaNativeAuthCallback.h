// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaLoginSubsystem.h"

#include "XsollaNativeAuthCallback.generated.h"

UCLASS()
class XSOLLALOGIN_API UXsollaNativeAuthCallback : public UObject
{
	GENERATED_BODY()

public:
	void BindSuccessDelegate(const FOnAuthUpdate& OnSuccess);
	void BindCancelDelegate(const FOnAuthCancel& OnCancel);
	void BindErrorDelegate(const FOnAuthError& OnError);

	void ExecuteSuccess(const FXsollaLoginData& LoginData);
	void ExecuteCancel();
	void ExecuteError(const FString& ErrorMessage);

private:
	UPROPERTY()
	FOnAuthUpdate OnAuthSuccessDelegate;
	UPROPERTY()
	FOnAuthCancel OnAuthCancelDelegate;
	UPROPERTY()
	FOnAuthError OnAuthErrorDelegate;
};
