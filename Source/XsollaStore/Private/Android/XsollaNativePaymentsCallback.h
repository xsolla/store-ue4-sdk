// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaStoreSubsystem.h"

#include "XsollaNativePaymentsCallback.generated.h"

UCLASS()
class XSOLLASTORE_API UXsollaNativePaymentsCallback : public UObject
{
	GENERATED_BODY()

public:
	void BindBrowserClosedDelegate(const FOnStoreBrowserClosed& OnBrowserClosed);

	void ExecuteBrowserClosed(bool bIsManually);

private:
	UPROPERTY()
	FOnStoreBrowserClosed OnBrowserClosedDelegate;

};
