// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreSave.h"

#include "Kismet/GameplayStatics.h"

const FString UXsollaStoreSave::SaveSlotName = "XsollaStoreSaveSlot";
const int32 UXsollaStoreSave::UserIndex = 0;

int32 UXsollaStoreSave::Load()
{
	auto SaveInstance = Cast<UXsollaStoreSave>(UGameplayStatics::LoadGameFromSlot(UXsollaStoreSave::SaveSlotName, UXsollaStoreSave::UserIndex));
	if (!SaveInstance)
	{
		return -1;
	}

	return SaveInstance->StoreCartId;
}

void UXsollaStoreSave::Save(const int32& InCartId)
{
	auto SaveInstance = Cast<UXsollaStoreSave>(UGameplayStatics::LoadGameFromSlot(UXsollaStoreSave::SaveSlotName, UXsollaStoreSave::UserIndex));
	if (!SaveInstance)
	{
		SaveInstance = Cast<UXsollaStoreSave>(UGameplayStatics::CreateSaveGameObject(UXsollaStoreSave::StaticClass()));
	}

	SaveInstance->StoreCartId = InCartId;

	UGameplayStatics::SaveGameToSlot(SaveInstance, UXsollaStoreSave::SaveSlotName, 0);
}
