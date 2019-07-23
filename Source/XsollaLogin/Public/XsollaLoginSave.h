// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginTypes.h"

#include "GameFramework/SaveGame.h"

#include "XsollaLoginSave.generated.h"

UCLASS()
class UXsollaLoginSave : public USaveGame
{
	GENERATED_BODY()

public:
	static FXsollaLoginData Load();
	static void Save(const FXsollaLoginData& InLoginData);

public:
	static const FString SaveSlotName;

	/** User index (always 0) */
	static const int32 UserIndex;

protected:
	UPROPERTY()
	FXsollaLoginData LoginData;
};
