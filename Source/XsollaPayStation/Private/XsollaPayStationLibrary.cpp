// Copyright 2019 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationLibrary.h"

#include "XsollaPayStation.h"
#include "XsollaPayStationController.h"
#include "XsollaPayStationSettings.h"

#include "Engine/Engine.h"

UXsollaPayStationLibrary::UXsollaPayStationLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaPayStationController* UXsollaPayStationLibrary::GetPayStationController(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return FXsollaPayStationModule::Get().GetPayStationController(World);
	}

	return nullptr;
}

UXsollaPayStationSettings* UXsollaPayStationLibrary::GetPayStationSettings()
{
	return FXsollaPayStationModule::Get().GetSettings();
}