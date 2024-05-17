// Copyright 2024 Xsolla Inc. All Rights Reserved.


#include "XsollaStoreBrowserWrapper.h"

void UXsollaStoreBrowserWrapper::ExecuteBrowserClosed(bool bIsManually)
{
	OnBrowserClosed.ExecuteIfBound(bIsManually);
}