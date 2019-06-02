// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStorePrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float amount;

	UPROPERTY(BlueprintReadOnly)
	FString currency;

public:
	FStorePrice(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString sku;

	UPROPERTY(BlueprintReadOnly)
	FString name;

	UPROPERTY(BlueprintReadOnly)
	FString description;

	UPROPERTY(BlueprintReadOnly)
	FString long_description;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> groups;

	UPROPERTY(BlueprintReadOnly)
	bool is_free;

	UPROPERTY(BlueprintReadOnly)
	TArray<FStorePrice> prices;

	UPROPERTY(BlueprintReadOnly)
	FString image_url;

public:
	FStoreItem(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FStoreItem> Items;

public:
	FStoreItemsData(){};
};
