// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaDevMenuSave.h"

#include "XsollaLoginSettings.h"
#include "Kismet/GameplayStatics.h"
#include "XsollaLogin.h"
#include "XsollaLoginTypes.h"


const FString UXsollaDevMenuSave::SaveSlotName = "XsollaDevMenuSaveSlot";
const int32 UXsollaDevMenuSave::UserIndex = 0;

FXsollaDevMenuData UXsollaDevMenuSave::LoadDevMenuData()
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		return FXsollaDevMenuData(Settings->LoginID, Settings->ProjectID, Settings->UseOAuth2, Settings->ClientID);
	}

	const UXsollaDevMenuSave* SaveInstance = Cast<UXsollaDevMenuSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if (!SaveInstance)
	{
		return FXsollaDevMenuData(Settings->LoginID, Settings->ProjectID, Settings->UseOAuth2, Settings->ClientID);
	}

	FXsollaDevMenuData CachedDevMenuData = SaveInstance->DevMenuData;

	return CachedDevMenuData;
}

void UXsollaDevMenuSave::SaveDevMenuData(const FString& InLoginId, const FString& InProjectId, const bool bInUseOAuth2, const FString& InClientId)
{
	UXsollaDevMenuSave* SaveInstance = Cast<UXsollaDevMenuSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if (!SaveInstance)
	{
		SaveInstance = Cast<UXsollaDevMenuSave>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
	}


	SaveInstance->DevMenuData = FXsollaDevMenuData(InLoginId, InProjectId, bInUseOAuth2, InClientId);

	UGameplayStatics::SaveGameToSlot(SaveInstance, SaveSlotName, 0);
}