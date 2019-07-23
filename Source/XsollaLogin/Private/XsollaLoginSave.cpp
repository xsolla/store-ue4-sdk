// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSave.h"

#include "Kismet/GameplayStatics.h"

const FString UXsollaLoginSave::SaveSlotName = "XsollaLoginSaveSlot";
const int32 UXsollaLoginSave::UserIndex = 0;

FXsollaLoginData UXsollaLoginSave::Load()
{
	auto SaveInstance = Cast<UXsollaLoginSave>(UGameplayStatics::LoadGameFromSlot(UXsollaLoginSave::SaveSlotName, UXsollaLoginSave::UserIndex));
	if (!SaveInstance)
	{
		return FXsollaLoginData();
	}

	return SaveInstance->LoginData;
}

void UXsollaLoginSave::Save(const FXsollaLoginData& InLoginData)
{
	auto SaveInstance = Cast<UXsollaLoginSave>(UGameplayStatics::LoadGameFromSlot(UXsollaLoginSave::SaveSlotName, UXsollaLoginSave::UserIndex));
	if (!SaveInstance)
	{
		SaveInstance = Cast<UXsollaLoginSave>(UGameplayStatics::CreateSaveGameObject(UXsollaLoginSave::StaticClass()));
	}

	SaveInstance->LoginData = InLoginData;

	UGameplayStatics::SaveGameToSlot(SaveInstance, UXsollaLoginSave::SaveSlotName, 0);
}
