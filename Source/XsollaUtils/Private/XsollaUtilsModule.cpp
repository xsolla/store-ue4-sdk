// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsModule.h"

#include "XsollaUtilsDefines.h"
#include "XsollaUtilsImageLoader.h"
#include "XsollaUtilsSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaUtilsModule"

void FXsollaUtilsModule::StartupModule()
{
	// Initialize image loader
	ImageLoader = NewObject<UXsollaUtilsImageLoader>();
	ImageLoader->AddToRoot();

	// Register settings
	XsollaUtilsSettings = NewObject<UXsollaUtilsSettings>(GetTransientPackage(), "XsollaUtilsSettings", RF_Standalone);
	XsollaUtilsSettings->AddToRoot();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaUtils",
			LOCTEXT("RuntimeSettingsName", "Xsolla Utils"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Utils"),
			XsollaUtilsSettings);
	}
}

void FXsollaUtilsModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaUtils");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		ImageLoader->RemoveFromRoot();
		XsollaUtilsSettings->RemoveFromRoot();
	}
	else
	{
		ImageLoader = nullptr;
		XsollaUtilsSettings = nullptr;
	}
}

UXsollaUtilsImageLoader* FXsollaUtilsModule::GetImageLoader()
{
	return ImageLoader;
}

UXsollaUtilsSettings* FXsollaUtilsModule::GetSettings() const
{
	check(XsollaUtilsSettings);
	return XsollaUtilsSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaUtilsModule, XsollaUtils);

DEFINE_LOG_CATEGORY(LogXsollaUtils);