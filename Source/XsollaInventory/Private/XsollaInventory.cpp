// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaInventory.h"

#include "XsollaInventoryDefines.h"
#include "XsollaInventorySettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaInventoryModule"

void FXsollaInventoryModule::StartupModule()
{
	XsollaInventorySettings = NewObject<UXsollaInventorySettings>(GetTransientPackage(), "XsollaInventorySettings", RF_Standalone);
	XsollaInventorySettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaInventory",
			LOCTEXT("RuntimeSettingsName", "Xsolla Inventory"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Inventory"),
			XsollaInventorySettings);
	}

	UE_LOG(LogXsollaInventory, Log, TEXT("%s: XsollaInventory module started"), *VA_FUNC_LINE);
}

void FXsollaInventoryModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaInventory");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaInventorySettings->RemoveFromRoot();
	}
	else
	{
		XsollaInventorySettings = nullptr;
	}
}

UXsollaInventorySettings* FXsollaInventoryModule::GetSettings() const
{
	check(XsollaInventorySettings);
	return XsollaInventorySettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaInventoryModule, XsollaInventory)

DEFINE_LOG_CATEGORY(LogXsollaInventory);
