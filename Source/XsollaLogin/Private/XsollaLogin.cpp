// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLogin.h"

#include "XsollaLoginController.h"
#include "XsollaLoginDefines.h"
#include "XsollaLoginSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/World.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

void FXsollaLoginModule::StartupModule()
{
	XsollaLoginSettings = NewObject<UXsollaLoginSettings>(GetTransientPackage(), "XsollaLoginSettings", RF_Standalone);
	XsollaLoginSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaLogin",
			LOCTEXT("RuntimeSettingsName", "Xsolla Login SDK"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Login SDK"),
			XsollaLoginSettings);
	}

	FWorldDelegates::OnWorldCleanup.AddLambda([this](UWorld* World, bool bSessionEnded, bool bCleanupResources) {
		XsollaLoginControllers.Remove(World);

		UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin Controller is removed for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

	FWorldDelegates::OnPostWorldInitialization.AddLambda([this](UWorld* World, const UWorld::InitializationValues IVS) {
		auto LoginController = NewObject<UXsollaLoginController>(GetTransientPackage());
		LoginController->SetFlags(RF_Standalone);
		LoginController->LoadSavedData();

		// Initialize module with project id provided by user
		const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
		LoginController->Initialize(Settings->LoginProjectID);

		XsollaLoginControllers.Add(World, LoginController);

		UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin Controller is created for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

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

	XsollaLoginControllers.Empty();
}

UXsollaLoginSettings* FXsollaLoginModule::GetSettings() const
{
	check(XsollaLoginSettings);
	return XsollaLoginSettings;
}

UXsollaLoginController* FXsollaLoginModule::GetLoginController(UWorld* World) const
{
	return XsollaLoginControllers.FindChecked(World);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaLoginModule, XsollaLogin)

DEFINE_LOG_CATEGORY(LogXsollaLogin);
