// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaInventoryLibrary.h"
#include "XsollaInventory.h"

UXsollaInventoryLibrary::UXsollaInventoryLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaInventorySettings* UXsollaInventoryLibrary::GetInventorySettings()
{
	return FXsollaInventoryModule::Get().GetSettings();
}