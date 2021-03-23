// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySettings.h"

UXsollaInventorySettings::UXsollaInventorySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UseCrossPlatformAccountLinking = false;
	WebStoreURL = TEXT("https://sitebuilder.xsolla.com/game/sdk-web-store/");
	DemoProjectID = TEXT("77640");
}
