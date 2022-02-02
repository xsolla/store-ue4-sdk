// Copyright 2022 Xsolla Inc. All Rights Reserved.

#include "XsollaProjectSettings.h"


UXsollaProjectSettings::UXsollaProjectSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UserDataStorage = EUserDataStorage::Xsolla;
	AccountLinkingURL = TEXT("https://livedemo.xsolla.com/sdk/sdk-shadow-account/link");
	PlatformAuthenticationURL = TEXT("https://livedemo.xsolla.com/sdk/sdk-shadow-account/auth");
	InvalidateExistingSessions = false;
	UseOAuth2 = true;
	UseCrossPlatformAccountLinking = false;
	EncryptCachedCredentials = false;
	SendPasswordlessAuthURL = false;
	CallbackURL = TEXT("https://login.xsolla.com/api/blank");
	Platform = EXsollaPublishingPlatform::playstation_network;
	bCustomAuthViaAccessToken = false;
	CustomAuthServerURL = "https://sdk.xsolla.com/";
	bAllowNativeAuth = false;
	PasswordlessAuthURL = TEXT("https://login-widget.xsolla.com/latest/confirm-status");
	EnableSandbox = true;
	UsePlatformBrowser = false;
	UseCrossPlatformAccountLinking = false;
	PaymentInterfaceTheme = EXsollaPaymentUiTheme::ps4_default_dark;
	PaymentInterfaceSize = EXsollaPaymentUiSize::medium;
	PaymentInterfaceVersion = EXsollaPaymentUiVersion::not_specified;
	OverrideRedirectPolicy = false;
	ReturnUrl = TEXT("");
	RedirectCondition = EXsollaPaymentRedirectCondition::none;
	RedirectDelay = 0;
	RedirectStatusManual = EXsollaPaymentRedirectStatusManual::none;
	RedirectButtonCaption = TEXT("");
}
