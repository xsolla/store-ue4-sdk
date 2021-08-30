// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsModule.h"

#include "XsollaUtilsDefines.h"
#include "XsollaUtilsImageLoader.h"

#define LOCTEXT_NAMESPACE "FXsollaUtilsModule"

const FName FXsollaUtilsModule::ModuleName = "XsollaUtils";

void FXsollaUtilsModule::StartupModule()
{
	// Initialize image loader
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