// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaLoginTypes.h"

#include "GameFramework/SaveGame.h"
#include "Runtime/Core/Public/Misc/AES.h"

#include "XsollaLoginSave.generated.h"

UCLASS()
class UXsollaLoginSave : public USaveGame
{
	GENERATED_BODY()

public:
	static FXsollaLoginData Load();
	static void Save(const FXsollaLoginData& InLoginData);

	static FAES::FAESKey GetEncryptionKey();
	static FString EncryptString(const FString& InString, const FAES::FAESKey& InKey);
	static FString DecryptString(const FString& InString, const FAES::FAESKey& InKey);

public:
	static const FString SaveSlotName;

	/** User index (always 0) */
	static const int32 UserIndex;

protected:
	UPROPERTY()
	FXsollaLoginData LoginData;
};
