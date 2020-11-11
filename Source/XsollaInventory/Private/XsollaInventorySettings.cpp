// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySettings.h"

UXsollaInventorySettings::UXsollaInventorySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UseCrossPlatformAccountLinking = false;
	WebStoreURL = TEXT("https://livedemo.xsolla.com/sdk-account-linking/");
	DemoProjectID = TEXT("77640");
}
