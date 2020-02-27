// Copyright 2019 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStation.h"

#include "XsollaPayStationDefines.h"
#include "XsollaPayStationSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaPayStationModule"

void FXsollaPayStationModule::StartupModule()
{
	XsollaPayStationSettings = NewObject<UXsollaPayStationSettings>(GetTransientPackage(), "XsollaPayStationSettings", RF_Standalone);
	XsollaPayStationSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaPayStation",
			LOCTEXT("RuntimeSettingsName", "Xsolla PayStation"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla PayStation"),
			XsollaPayStationSettings);
	}

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: XsollaPayStation module started"), *VA_FUNC_LINE);
}

void FXsollaPayStationModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaPayStation");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaPayStationSettings->RemoveFromRoot();
	}
	else
	{
		XsollaPayStationSettings = nullptr;
	}
}

UXsollaPayStationSettings* FXsollaPayStationModule::GetSettings() const
{
	check(XsollaPayStationSettings);
	return XsollaPayStationSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaPayStationModule, XsollaPayStation)

DEFINE_LOG_CATEGORY(LogXsollaPayStation);