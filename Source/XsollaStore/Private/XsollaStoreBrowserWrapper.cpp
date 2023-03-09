// Copyright 2023 Xsolla Inc. All Rights Reserved.


#include "XsollaStoreBrowserWrapper.h"

void UXsollaStoreBrowserWrapper::ExecuteBrowserClosed()
{
	OnBrowserClosed.ExecuteIfBound();
}