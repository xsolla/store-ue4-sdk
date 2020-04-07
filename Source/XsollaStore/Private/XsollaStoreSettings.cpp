// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreSettings.h"

UXsollaStoreSettings::UXsollaStoreSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSandbox = true;
	bEnableSandboxInShipping = false;
	bUsePlatformBrowser = false;
	bBuildForSteam = false;
	DemoProjectId = TEXT("44056");
	PaymentInterfaceTheme = EXsollaPaymentUiTheme::Dark;
	PublishingPlatform = EXsollaPublishingPlatform::NotDefined;
}
