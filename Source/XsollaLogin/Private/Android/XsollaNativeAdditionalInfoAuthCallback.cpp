// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaNativeAdditionalInfoAuthCallback.h"

#include "Async/Async.h"
#include "XsollaLoginDefines.h"

void UXsollaNativeAdditionalInfoAuthCallback::BindSuccessDelegate(const FOnAuthUpdate& OnSuccess, UXsollaLoginSubsystem* InLoginSubsystem)
{
	OnAuthSuccessDelegate = OnSuccess;
	LoginSubsystem = InLoginSubsystem;
	bTerminalDispatched.store(false);
	AddToRoot();
}

void UXsollaNativeAdditionalInfoAuthCallback::BindErrorDelegate(const FOnAuthError& OnError)
{
	OnAuthErrorDelegate = OnError;
}

void UXsollaNativeAdditionalInfoAuthCallback::ExecuteSuccess(const FString& AuthenticationCode, const FString& AuthenticationToken)
{
	if (bTerminalDispatched.exchange(true))
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Duplicate additional-info success callback ignored"), *VA_FUNC_LINE);
		return;
	}

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

		if (IsRooted())
		{
			RemoveFromRoot();
		}
	});
}

void UXsollaNativeAdditionalInfoAuthCallback::ExecuteCancel()
{
	if (bTerminalDispatched.exchange(true))
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Duplicate additional-info cancel callback ignored"), *VA_FUNC_LINE);
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if (IsValid(LoginSubsystem))
		{
			LoginSubsystem->HandleAdditionalInfoAuthCancel(OnAuthErrorDelegate);
		}
		else
		{
			OnAuthErrorDelegate.ExecuteIfBound(TEXT(""), TEXT("Additional fields form submission was canceled by user"));
		}

		if (IsRooted())
		{
			RemoveFromRoot();
		}
	});
}

void UXsollaNativeAdditionalInfoAuthCallback::ExecuteError(const FString& ErrorCode, const FString& ErrorMessage)
{
	if (bTerminalDispatched.exchange(true))
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Duplicate additional-info error callback ignored"), *VA_FUNC_LINE);
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, ErrorCode, ErrorMessage]()
	{
		const FString NormalizedErrorMessage = ErrorMessage.IsEmpty() ? TEXT("Additional fields form submission failed") : ErrorMessage;
		if (IsValid(LoginSubsystem))
		{
			LoginSubsystem->HandleAdditionalInfoAuthError(ErrorCode, NormalizedErrorMessage, OnAuthErrorDelegate);
		}
		else
		{
			OnAuthErrorDelegate.ExecuteIfBound(ErrorCode, NormalizedErrorMessage);
		}

		if (IsRooted())
		{
			RemoveFromRoot();
		}
	});
}
