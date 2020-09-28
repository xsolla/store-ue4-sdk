// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSettings.h"

UXsollaLoginSettings::UXsollaLoginSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UserDataStorage = EUserDataStorage::Xsolla;
	AccountLinkingURL = TEXT("https://livedemo.xsolla.com/sdk/shadow_account/link");
	PlatformAuthenticationURL = TEXT("https://livedemo.xsolla.com/sdk/shadow_account/auth");
	InvalidateExistingSessions = false;
	UseOAuth2 = false;
	UseCrossPlatformAccountLinking = false;
	DemoProjectID = TEXT("44056");
	DemoLoginID = TEXT("e6dfaac6-78a8-11e9-9244-42010aa80004");
	EncryptCachedCredentials = false;
	RequestNickname = false;
}
