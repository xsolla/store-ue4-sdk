// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStore.h"

#include "XsollaStoreDefines.h"
#include "XsollaStoreSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

const FName FXsollaStoreModule::ModuleName = "XsollaStore";

void FXsollaStoreModule::StartupModule()
{
	XsollaStoreSettings = NewObject<UXsollaStoreSettings>(GetTransientPackage(), "XsollaStoreSettings", RF_Standalone);
	XsollaStoreSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", ModuleName,
			LOCTEXT("RuntimeSettingsName", "Xsolla Store"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Store"),
			XsollaStoreSettings);
	}

	UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore module started"), *VA_FUNC_LINE);
}

void FXsollaStoreModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", ModuleName);
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaStoreSettings->RemoveFromRoot();
	}
	else
	{
		XsollaStoreSettings = nullptr;
	}
}

UXsollaStoreSettings* FXsollaStoreModule::GetSettings() const
{
	check(XsollaStoreSettings);
	return XsollaStoreSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaStoreModule, XsollaStore)

DEFINE_LOG_CATEGORY(LogXsollaStore);
