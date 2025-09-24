// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaProjectSettings.h"

UXsollaProjectSettings::UXsollaProjectSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProjectID = TEXT("77640");
	LoginID = TEXT("026201e3-7e40-11ea-a85b-42010aa80004");
	ClientID = TEXT("57");
	EncryptCachedCredentials = false;
	SendPasswordlessAuthURL = false;
	RedirectURI = TEXT("https://login.xsolla.com/api/blank");
	PasswordlessAuthURL = TEXT("https://login-widget.xsolla.com/latest/confirm-status");
	WebStoreURL = TEXT("https://sdk-web-store.xsolla.site/");
	EnableSandbox = true;
	UsePlatformBrowser = false;
	BuildForSteam = false;
	PaymentInterfaceThemeId = TEXT("63295aab2e47fab76f7708e3");
	UseSettingsFromPublisherAccount = true;
	ReturnUrl = TEXT("");
	RedirectCondition = EXsollaPaymentRedirectCondition::none;
	RedirectDelay = 0;
	RedirectStatusManual = EXsollaPaymentRedirectStatusManual::none;
	RedirectButtonCaption = TEXT("");
}
