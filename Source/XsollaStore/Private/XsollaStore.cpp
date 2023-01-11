// Copyright 2023 Xsolla Inc. All Rights Reserved.

#include "XsollaStore.h"

#include "XsollaStoreDefines.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

const FName FXsollaStoreModule::ModuleName = "XsollaStore";

void FXsollaStoreModule::StartupModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}
	
	UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore module started"), *VA_FUNC_LINE);
}

void FXsollaStoreModule::ShutdownModule()
{
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaStoreModule, XsollaStore)

DEFINE_LOG_CATEGORY(LogXsollaStore);
