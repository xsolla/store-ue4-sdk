// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaStoreSubsystem.h"

#include "XsollaNativePaymentsCallback.generated.h"

UCLASS()
class XSOLLASTORE_API UXsollaNativePaymentsCallback : public UObject
{
	GENERATED_BODY()

public:
	void BindSuccessDelegate(const FOnStoreSuccessPayment& OnSuccess);
	void BindErrorDelegate(const FOnError& OnError);
	void BindCancelDelegate(const FOnStoreCancelPayment& OnCancel);

	void ExecuteSuccess();
	void ExecuteError(const FString& ErrorMessage);
	void ExecuteCancel();

private:
	UPROPERTY()
	FOnStoreSuccessPayment OnPaymentsSuccessDelegate;

	UPROPERTY()
	FOnError OnPaymentsErrorDelegate;

	UPROPERTY()
	FOnStoreCancelPayment OnPaymentsCancelDelegate;
};
