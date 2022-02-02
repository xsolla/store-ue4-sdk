// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaInventory.h"

#include "XsollaInventoryDefines.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaInventoryModule"

void FXsollaInventoryModule::StartupModule()
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: XsollaInventory module started"), *VA_FUNC_LINE);
}

void FXsollaInventoryModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaInventoryModule, XsollaInventory)

DEFINE_LOG_CATEGORY(LogXsollaInventory);
