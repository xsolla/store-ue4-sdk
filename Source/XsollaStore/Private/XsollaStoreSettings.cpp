// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaStoreSettings.h"

UXsollaStoreSettings::UXsollaStoreSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetupDefaultDemoSettings();
}

void UXsollaStoreSettings::SetupDefaultDemoSettings()
{
	EnableSandbox = true;
	EnableSandboxInShippingBuild = false;
	UsePlatformBrowser = false;
	BuildForSteam = false;
	UseCrossPlatformAccountLinking = false;
	DemoProjectID = TEXT("77640");
	PaymentInterfaceTheme = EXsollaPaymentUiTheme::dark;
	PaymentInterfaceSize = EXsollaPaymentUiSize::medium;
	PaymentInterfaceVersion = EXsollaPaymentUiVersion::not_specified;
	OverrideRedirectPolicy = false;
	ReturnUrl = TEXT("");
	RedirectCondition = EXsollaPaymentRedirectCondition::none;
	RedirectDelay = 0;
	RedirectStatusManual = EXsollaPaymentRedirectStatusManual::none;
	RedirectButtonCaption = TEXT("");
}