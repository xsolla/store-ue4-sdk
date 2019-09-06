// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginLibrary.h"

#include "XsollaLogin.h"

#include "Engine/Engine.h"

UXsollaLoginLibrary::UXsollaLoginLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaLoginController* UXsollaLoginLibrary::GetLoginController(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return FXsollaLoginModule::Get().GetLoginController(World);
	}

	return nullptr;
}

UXsollaLoginSettings* UXsollaLoginLibrary::GetLoginSettings()
{
	return FXsollaLoginModule::Get().GetSettings();
}
