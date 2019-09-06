// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "GameFramework/SaveGame.h"

#include "XsollaStoreDefines.h"

#include "XsollaStoreSave.generated.h"

USTRUCT(Blueprintable)
struct XSOLLASTORE_API FXsollaStoreSaveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 CartId;

	UPROPERTY()
	FString CartCurrency;

	FXsollaStoreSaveData()
		: CartId(INVALID_CART)
		, CartCurrency(TEXT("USD")){};

	FXsollaStoreSaveData(int32 InCartId, const FString& InCartCurrency)
		: CartId(InCartId)
		, CartCurrency(InCartCurrency){};
};

UCLASS()
class UXsollaStoreSave : public USaveGame
{
	GENERATED_BODY()

public:
	static FXsollaStoreSaveData Load();
	static void Save(const FXsollaStoreSaveData& InCartData);

public:
	static const FString SaveSlotName;

	/** User index (always 0) */
	static const int32 UserIndex;

protected:
	UPROPERTY()
	FXsollaStoreSaveData CartData;
};
