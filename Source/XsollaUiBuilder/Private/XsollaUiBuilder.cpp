// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaUiBuilder.h"

#include "UObject/Package.h"
#include "XsollaUiBuilderSettings.h"
#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaUiBuilderModule"

void FXsollaUiBuilderModule::StartupModule()
{
	XsollaUiBuilderSettings = NewObject<UXsollaUiBuilderSettings>(GetTransientPackage(), "XsollaUiBuilderSettings", RF_Standalone);
	XsollaUiBuilderSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaUiBuilder",
			LOCTEXT("RuntimeSettingsName", "Xsolla UI Builder"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla UI Builder"),
			XsollaUiBuilderSettings);
	}
}

void FXsollaUiBuilderModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaUiBuilder");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaUiBuilderSettings->RemoveFromRoot();
	}
	else
	{
		XsollaUiBuilderSettings = nullptr;
	}
}

UXsollaUiBuilderSettings* FXsollaUiBuilderModule::GetSettings() const
{
	check(XsollaUiBuilderSettings);
	return XsollaUiBuilderSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaUiBuilderModule, XsollaUiBuilder)
