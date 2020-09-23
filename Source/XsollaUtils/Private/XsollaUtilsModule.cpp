// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsModule.h"

#include "XsollaUtilsDefines.h"
#include "XsollaUtilsImageLoader.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FXsollaUtilsModule"

void FXsollaUtilsModule::StartupModule()
{
	ImageLoader = NewObject<UXsollaUtilsImageLoader>();
	ImageLoader->AddToRoot();
}

void FXsollaUtilsModule::ShutdownModule()
{
	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		ImageLoader->RemoveFromRoot();
	}
	else
	{
		ImageLoader = nullptr;
	}
}

UXsollaUtilsImageLoader* FXsollaUtilsModule::GetImageLoader()
{
	return ImageLoader;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaUtilsModule, XsollaUtils);

DEFINE_LOG_CATEGORY(LogXsollaUtils);