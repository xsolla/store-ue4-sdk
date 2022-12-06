// Copyright 2022 Xsolla Inc. All Rights Reserved.


#include "XsollaStoreBrowserWrapper.h"

void UXsollaStoreBrowserWrapper::ExecuteBrowserClosed()
{
	OnBrowserClosed.ExecuteIfBound();
}