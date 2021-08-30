// Copyright 2021 Xsolla Inc. All Rights Reserved.

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