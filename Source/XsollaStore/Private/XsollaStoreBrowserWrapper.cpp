// Copyright 2022 Xsolla Inc. All Rights Reserved.


#include "XsollaStoreBrowserWrapper.h"

void UXsollaStoreBrowserWrapper::ExecuteSuccess()
{
	OnSuccess.ExecuteIfBound();
}

void UXsollaStoreBrowserWrapper::ExecuteError(const FString& ErrorMessage)
{
	OnError.ExecuteIfBound(ErrorMessage);
}

void UXsollaStoreBrowserWrapper::ExecuteCancel()
{
	OnCancel.ExecuteIfBound();
}