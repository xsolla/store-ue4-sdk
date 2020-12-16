// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreSave.h"

#include "XsollaStoreDefines.h"

#include "Kismet/GameplayStatics.h"

const FString UXsollaStoreSave::SaveSlotName = "XsollaStoreSaveSlot";
const int32 UXsollaStoreSave::UserIndex = 0;

FXsollaStoreSaveData UXsollaStoreSave::Load()
{
	if(!UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		return FXsollaStoreSaveData();		
	}
	
	UXsollaStoreSave* SaveInstance = Cast<UXsollaStoreSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if (!SaveInstance)
	{
		return FXsollaStoreSaveData();
	}

	return SaveInstance->CartData;
}

void UXsollaStoreSave::Save(const FXsollaStoreSaveData& InCartData)
{
	auto SaveInstance = Cast<UXsollaStoreSave>(UGameplayStatics::LoadGameFromSlot(UXsollaStoreSave::SaveSlotName, UXsollaStoreSave::UserIndex));
	if (!SaveInstance)
	{
		SaveInstance = Cast<UXsollaStoreSave>(UGameplayStatics::CreateSaveGameObject(UXsollaStoreSave::StaticClass()));
	}

	SaveInstance->CartData = InCartData;

	UGameplayStatics::SaveGameToSlot(SaveInstance, UXsollaStoreSave::SaveSlotName, 0);
}
