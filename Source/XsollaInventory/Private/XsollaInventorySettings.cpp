// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySettings.h"

UXsollaInventorySettings::UXsollaInventorySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UseCrossPlatformAccountLinking = false;
	DemoProjectID = TEXT("44056");
}
