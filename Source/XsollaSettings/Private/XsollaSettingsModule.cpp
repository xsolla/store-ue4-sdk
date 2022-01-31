// Copyright 2022 Xsolla Inc. All Rights Reserved.

#include "XsollaSettingsModule.h"

#include "XsollaSettingsDefines.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "XsollaSettings.h"

#define LOCTEXT_NAMESPACE "FXsollaSettingsModule"

const FName FXsollaSettingsModule::ModuleName = "XsollaSettings";

void FXsollaSettingsModule::StartupModule()
{
	XsollaSettings = NewObject<UXsollaSettings>(GetTransientPackage(), "XsollaSettings", RF_Standalone);
	XsollaSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", ModuleName,
			LOCTEXT("RuntimeSettingsName", "Xsolla Settings"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Settings"),
			XsollaSettings);
	}

	UE_LOG(LogXsollaSettings, Log, TEXT("%s: XsollaSettings module started"), *VA_FUNC_LINE);
}

void FXsollaSettingsModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", ModuleName);
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaSettings->RemoveFromRoot();
	}
	else
	{
		XsollaSettings = nullptr;
	}
}

UXsollaSettings* FXsollaSettingsModule::GetSettings() const
{
	check(XsollaSettings);
	return XsollaSettings;
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaSettingsModule, XsollaSettings);

DEFINE_LOG_CATEGORY(LogXsollaSettings);
