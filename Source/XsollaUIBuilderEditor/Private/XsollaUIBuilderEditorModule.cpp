// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderEditorModule.h"
#include "XsollaUIBuilderSettingsDetails.h"
#include "XsollaGenericWrapperDetails.h"
#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaUIBuilderEditorModule"

const FName FXsollaUIBuilderEditorModule::ModuleName = "XsollaUIBuilderEditor";

void FXsollaUIBuilderEditorModule::StartupModule()
{
	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		"XsollaUIBuilderSettings",
		FOnGetDetailCustomizationInstance::CreateStatic(&FXsollaUIBuilderSettingsDetails::MakeInstance));

	PropertyModule.RegisterCustomClassLayout(
		"XsollaGenericWrapper",
		FOnGetDetailCustomizationInstance::CreateStatic(&FXsollaGenericWrapperDetails::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FXsollaUIBuilderEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.UnregisterCustomClassLayout("XsollaUIBuilderSettings");
		PropertyModule.UnregisterCustomClassLayout("XsollaGenericWrapper");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaUIBuilderEditorModule, XsollaUIBuilderEditor);

//DEFINE_LOG_CATEGORY(LogXsollaUIBuilderEditor);