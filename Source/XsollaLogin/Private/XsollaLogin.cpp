// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLogin.h"

#include "XsollaLoginDefines.h"
#include "XsollaLoginSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

void FXsollaLoginModule::StartupModule()
{
	XsollaLoginSettings = NewObject<UXsollaLoginSettings>(GetTransientPackage(), "XsollaLoginSettings", RF_Standalone);
	XsollaLoginSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaLogin",
			LOCTEXT("RuntimeSettingsName", "Xsolla Login"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Login"),
			XsollaLoginSettings);
	}

	UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin module started"), *VA_FUNC_LINE);
}

void FXsollaLoginModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaLogin");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaLoginSettings->RemoveFromRoot();
	}
	else
	{
		XsollaLoginSettings = nullptr;
	}
}

UXsollaLoginSettings* FXsollaLoginModule::GetSettings() const
{
	check(XsollaLoginSettings);
	return XsollaLoginSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaLoginModule, XsollaLogin)

DEFINE_LOG_CATEGORY(LogXsollaLogin);
