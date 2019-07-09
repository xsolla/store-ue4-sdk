// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "GameFramework/SaveGame.h"

#include "XsollaStoreSave.generated.h"

UCLASS()
class UXsollaStoreSave : public USaveGame
{
	GENERATED_BODY()

public:
	static int32 Load();
	static void Save(const int32& InCartId);

public:
	static const FString SaveSlotName;

	/** User index (always 0) */
	static const int32 UserIndex;

protected:
	UPROPERTY()
	int32 StoreCartId;
};
