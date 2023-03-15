// Copyright 2023 Xsolla Inc. All Rights Reserved.

#include "XsollaNativePaymentsCallback.h"

#include "Async/Async.h"

void UXsollaNativePaymentsCallback::BindBrowserClosedDelegate(const FOnStoreBrowserClosed& OnBrowserClosed)
{
	OnBrowserClosedDelegate = OnBrowserClosed;
}

void UXsollaNativePaymentsCallback::ExecuteBrowserClosed(bool bIsManually)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
		{ OnBrowserClosedDelegate.ExecuteIfBound(bIsManually); });
}