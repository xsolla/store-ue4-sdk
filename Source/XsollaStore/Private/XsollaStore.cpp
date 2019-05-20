// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStore.h"

#include "XsollaStoreController.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/World.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

void FXsollaStoreModule::StartupModule()
{
	XsollaStoreSettings = NewObject<UXsollaStoreSettings>(GetTransientPackage(), "XsollaStoreSettings", RF_Standalone);
	XsollaStoreSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaStore",
			LOCTEXT("RuntimeSettingsName", "Xsolla Store SDK"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Store SDK"),
			XsollaStoreSettings);
	}

	FWorldDelegates::OnWorldCleanup.AddLambda([this](UWorld* World, bool bSessionEnded, bool bCleanupResources) {
		XsollaStoreControllers.Remove(World);

		UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore Controller is removed for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

	FWorldDelegates::OnPostWorldInitialization.AddLambda([this](UWorld* World, const UWorld::InitializationValues IVS) {
		auto StoreController = NewObject<UXsollaStoreController>(GetTransientPackage());
		StoreController->SetFlags(RF_Standalone);

		// Initialize module with project id provided by user
		const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
		StoreController->Initialize(Settings->ProjectId);

		XsollaStoreControllers.Add(World, StoreController);

		UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore Controller is created for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

	UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore module started"), *VA_FUNC_LINE);
}

void FXsollaStoreModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaStore");
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

	XsollaStoreControllers.Empty();
}

UXsollaStoreSettings* FXsollaStoreModule::GetSettings() const
{
	check(XsollaStoreSettings);
	return XsollaStoreSettings;
}

UXsollaStoreController* FXsollaStoreModule::GetStoreController(UWorld* World) const
{
	return XsollaStoreControllers.FindChecked(World);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaStoreModule, XsollaStore)

DEFINE_LOG_CATEGORY(LogXsollaStore);
