// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaLogin.h"

#include "XsollaLoginDefines.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

const FName FXsollaLoginModule::ModuleName = "XsollaLogin";

void FXsollaLoginModule::StartupModule()
{
	UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin module started"), *VA_FUNC_LINE);
}

void FXsollaLoginModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaLoginModule, XsollaLogin)

DEFINE_LOG_CATEGORY(LogXsollaLogin);
