// Copyright 2019 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStation.h"

#include "XsollaPayStationController.h"
#include "XsollaPayStationDefines.h"
#include "XsollaPayStationSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/World.h"
#include "UObject/Package.h"

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

	FWorldDelegates::OnWorldCleanup.AddLambda([this](UWorld* World, bool bSessionEnded, bool bCleanupResources) {
		XsollaPayStationControllers.Remove(World);

		UE_LOG(LogXsollaPayStation, Log, TEXT("%s: XsollaPayStation Controller is removed for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

	FWorldDelegates::OnPostWorldInitialization.AddLambda([this](UWorld* World, const UWorld::InitializationValues IVS) {
		auto PayStationController = NewObject<UXsollaPayStationController>(GetTransientPackage());
		PayStationController->SetFlags(RF_Standalone);
		PayStationController->AddToRoot();

		XsollaPayStationControllers.Add(World, PayStationController);

		UE_LOG(LogXsollaPayStation, Log, TEXT("%s: XsollaPayStation Controller is created for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

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

		for (auto PayStationController : XsollaPayStationControllers)
		{
			PayStationController.Value->RemoveFromRoot();
		}
	}
	else
	{
		XsollaPayStationSettings = nullptr;
	}

	XsollaPayStationControllers.Empty();
}

UXsollaPayStationSettings* FXsollaPayStationModule::GetSettings() const
{
	check(XsollaPayStationSettings);
	return XsollaPayStationSettings;
}

UXsollaPayStationController* FXsollaPayStationModule::GetPayStationController(UWorld* World) const
{
	return XsollaPayStationControllers.FindChecked(World);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaPayStationModule, XsollaPayStation)

DEFINE_LOG_CATEGORY(LogXsollaPayStation);