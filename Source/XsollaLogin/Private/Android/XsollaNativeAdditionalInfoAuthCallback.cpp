// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaNativeAdditionalInfoAuthCallback.h"

#include "Async/Async.h"

void UXsollaNativeAdditionalInfoAuthCallback::BindSuccessDelegate(const FOnAuthUpdate& OnSuccess, UXsollaLoginSubsystem* InLoginSubsystem)
{
	OnAuthSuccessDelegate = OnSuccess;
	LoginSubsystem = InLoginSubsystem;
}

void UXsollaNativeAdditionalInfoAuthCallback::BindErrorDelegate(const FOnAuthError& OnError)
{
	OnAuthErrorDelegate = OnError;
}

void UXsollaNativeAdditionalInfoAuthCallback::ExecuteSuccess(const FString& AuthenticationCode, const FString& AuthenticationToken)
{
	AsyncTask(ENamedThreads::GameThread, [this, AuthenticationCode, AuthenticationToken]()
	{
		if (IsValid(LoginSubsystem))
		{
			LoginSubsystem->HandleAdditionalInfoAuthResult(AuthenticationCode, AuthenticationToken, OnAuthSuccessDelegate, OnAuthErrorDelegate);
		}
		else
		{
			OnAuthErrorDelegate.ExecuteIfBound(TEXT("0"), TEXT("Login subsystem is not valid"));
		}
	});
}

void UXsollaNativeAdditionalInfoAuthCallback::ExecuteCancel()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnAuthErrorDelegate.ExecuteIfBound(TEXT(""), TEXT("Additional fields form submission was canceled by user"));
	});
}

void UXsollaNativeAdditionalInfoAuthCallback::ExecuteError(const FString& ErrorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [this, ErrorMessage]()
	{
		const FString NormalizedErrorMessage = ErrorMessage.IsEmpty() ? TEXT("Additional fields form submission failed") : ErrorMessage;
		OnAuthErrorDelegate.ExecuteIfBound(TEXT("010-017"), NormalizedErrorMessage);
	});
}
