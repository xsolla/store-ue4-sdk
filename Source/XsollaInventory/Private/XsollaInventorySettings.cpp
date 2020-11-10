// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySettings.h"

UXsollaInventorySettings::UXsollaInventorySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UseCrossPlatformAccountLinking = false;
	WebStoreURL = TEXT("https://sitebuilder.xsolla.com/preview/sdk-web-store");
	DemoProjectID = TEXT("44056");
}
