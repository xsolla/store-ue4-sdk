// Copyright 2024 Xsolla Inc. All Rights Reserved.


#include "XsollaSocialLinkingBrowserWrapper.h"

void UXsollaSocialLinkingBrowserWrapper::ExecuteBrowserClosed(bool bIsManually, const FString& Token, const FString& ErrorCode, const FString& ErrorDescription)
{
	OnBrowserClosed.ExecuteIfBound(bIsManually, Token, ErrorCode, ErrorDescription);
}