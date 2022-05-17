// Copyright 2022 Xsolla Inc. All Rights Reserved.


#include "XsollaWebBrowserWrapper.h"

void UXsollaWebBrowserWrapper::BrowserClosed()
{
	OnBrowserClosed.ExecuteIfBound();
}
