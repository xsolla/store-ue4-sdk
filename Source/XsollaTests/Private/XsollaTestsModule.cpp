// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaTestsModule.h"
#include "XsollaTestsDefines.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FXsollaTestsModule"

void FXsollaTestsModule::StartupModule()
{
}

void FXsollaTestsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaTestsModule, XsollaTests);

DEFINE_LOG_CATEGORY(LogXsollaTests);