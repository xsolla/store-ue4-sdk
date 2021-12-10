// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaLoginSettings.h"

#include "XsollaUtilsDataModel.h"

UXsollaLoginSettings::UXsollaLoginSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetupDefaultDemoSettings();
}

void UXsollaLoginSettings::SetupDefaultDemoSettings()
{
	UserDataStorage = EUserDataStorage::Xsolla;
	AccountLinkingURL = TEXT("https://livedemo.xsolla.com/sdk/sdk-shadow-account/link");
	PlatformAuthenticationURL = TEXT("https://livedemo.xsolla.com/sdk/sdk-shadow-account/auth");
	InvalidateExistingSessions = false;
	UseOAuth2 = true;
	UseCrossPlatformAccountLinking = false;
	DemoProjectID = TEXT("77640");
	DemoLoginID = TEXT("026201e3-7e40-11ea-a85b-42010aa80004");
	DemoClientID = TEXT("57");
	EncryptCachedCredentials = false;
	SendPasswordlessAuthURL = false;
	RequestNickname = true;
	CallbackURL = TEXT("https://login.xsolla.com/api/blank");
	bForceSteamAuthorization = false;
	Platform = EXsollaPublishingPlatform::playstation_network;
	bCustomAuthViaAccessToken = false;
	CustomAuthServerURL = "https://sdk.xsolla.com/";
	bAllowNativeAuth = false;
	PasswordlessAuthURL = TEXT("https://login-widget.xsolla.com/latest/confirm-status");
}
