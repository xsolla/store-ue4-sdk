// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaInventoryLibrary.h"
#include "XsollaInventory.h"

#include "Kismet/KismetTextLibrary.h"
#include "UObject/ConstructorHelpers.h"

UXsollaInventoryLibrary::UXsollaInventoryLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaInventorySettings* UXsollaInventoryLibrary::GetInventorySettings()
{
	return FXsollaInventoryModule::Get().GetSettings();
}