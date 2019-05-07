// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreLibrary.h"

#include "XsollaStore.h"

#include "Engine/Engine.h"

UXsollaStoreLibrary::UXsollaStoreLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaStoreController* UXsollaStoreLibrary::GetStoreController(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return FXsollaStoreModule::Get().GetStoreController(World);
	}

	return nullptr;
}

void UXsollaStoreLibrary::UpdateVirtualItems(UObject* WorldContextObject, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	UXsollaStoreLibrary::GetStoreController(WorldContextObject)->UpdateVirtualItems(SuccessCallback, ErrorCallback);
}
