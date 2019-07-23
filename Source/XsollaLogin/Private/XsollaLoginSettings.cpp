// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSettings.h"

UXsollaLoginSettings::UXsollaLoginSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UserDataStorage = EUserDataStorage::Xsolla;
}
