// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSettings.h"

UXsollaLoginSettings::UXsollaLoginSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UserDataStorage = EUserDataStorage::Xsolla;
	DemoProjectId = TEXT("44056");
	DemoLoginProjectID = TEXT("e6dfaac6-78a8-11e9-9244-42010aa80004");
}
