// Copyright 2019 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationSettings.h"

UXsollaPayStationSettings::UXsollaPayStationSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TokenRequestURL = TEXT("https://livedemo.xsolla.com/paystation/token_unreal.php");
	EnableSandbox = false;
	EnableSandboxInShippingBuild = false;
}