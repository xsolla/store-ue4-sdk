// Copyright 2023 Xsolla Inc. All Rights Reserved.


#include "XsollaLoginBrowserWrapper.h"

void UXsollaLoginBrowserWrapper::ExecuteBrowserClosed(bool bIsManually, const FString& AuthenticationCode)
{
	OnBrowserClosed.ExecuteIfBound(bIsManually, AuthenticationCode);
}