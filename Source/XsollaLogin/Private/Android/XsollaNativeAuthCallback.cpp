// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaNativeAuthCallback.h"

#include "Async/Async.h"

void UXsollaNativeAuthCallback::BindSuccessDelegate(const FOnAuthUpdate& OnSuccess)
{
	OnAuthSuccessDelegate = OnSuccess;
}

void UXsollaNativeAuthCallback::BindCancelDelegate(const FOnAuthCancel& OnCancel)
{
	OnAuthCancelDelegate = OnCancel;
}

void UXsollaNativeAuthCallback::BindErrorDelegate(const FOnAuthError& OnError)
{
	OnAuthErrorDelegate = OnError;
}

void UXsollaNativeAuthCallback::ExecuteSuccess(const FXsollaLoginData& LoginData)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnAuthSuccessDelegate.ExecuteIfBound(LoginData);
	});
}

void UXsollaNativeAuthCallback::ExecuteCancel()
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnAuthCancelDelegate.ExecuteIfBound();
	});
}

void UXsollaNativeAuthCallback::ExecuteError(const FString& ErrorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnAuthErrorDelegate.ExecuteIfBound(TEXT("0"), ErrorMessage);
	});
}
