// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaNativePaymentsCallback.h"

#include "Async/Async.h"

void UXsollaNativePaymentsCallback::BindSuccessDelegate(const FOnStoreSuccessPayment& OnSuccess)
{
	OnPaymentsSuccessDelegate = OnSuccess;
}

void UXsollaNativePaymentsCallback::BindErrorDelegate(const FOnError& OnError)
{
	OnPaymentsErrorDelegate = OnError;
}

void UXsollaNativePaymentsCallback::BindCancelDelegate(const FOnStoreCancelPayment& OnCancel)
{
	OnPaymentsCancelDelegate = OnCancel;
}

void UXsollaNativePaymentsCallback::ExecuteSuccess()
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnPaymentsSuccessDelegate.ExecuteIfBound();
	});
}

void UXsollaNativePaymentsCallback::ExecuteError(const FString& ErrorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnPaymentsErrorDelegate.ExecuteIfBound(0, 0, ErrorMessage);
	});
}

void UXsollaNativePaymentsCallback::ExecuteCancel()
{
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnPaymentsCancelDelegate.ExecuteIfBound(); 
		});
}