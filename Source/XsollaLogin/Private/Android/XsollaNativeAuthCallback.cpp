// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaNativeAuthCallback.h"

#include "Async/Async.h"

void UXsollaNativeAuthCallback::BindSuccessDelegate(const FOnAuthUpdate& OnSuccess, UXsollaLoginSubsystem* InLoginSubsystem)
{
	OnAuthSuccessDelegate = OnSuccess;
	LoginSubsystem = InLoginSubsystem;
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
	AsyncTask(ENamedThreads::GameThread, [this, LoginData]()
		{ 
		LoginSubsystem->SetLoginData(LoginData);
		OnAuthSuccessDelegate.ExecuteIfBound(LoginData);
	});
}

void UXsollaNativeAuthCallback::ExecuteCancel()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		OnAuthCancelDelegate.ExecuteIfBound();
	});
}

void UXsollaNativeAuthCallback::ExecuteError(const FString& ErrorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [this, ErrorMessage]() {
		OnAuthErrorDelegate.ExecuteIfBound(TEXT("0"), ErrorMessage);
	});
}
