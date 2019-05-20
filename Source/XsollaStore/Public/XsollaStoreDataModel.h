// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDataModel.generated.h"

USTRUCT(BlueprintType)
struct FStorePrice
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float amount;

	UPROPERTY(VisibleAnywhere)
	FString currency;

public:
	FStorePrice(){};
};

USTRUCT(BlueprintType)
struct FStoreItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FString sku;

	UPROPERTY(VisibleAnywhere)
	FString description;

	UPROPERTY(VisibleAnywhere)
	FString long_description;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> groups;

	UPROPERTY(VisibleAnywhere)
	bool is_free;

	UPROPERTY(VisibleAnywhere)
	TArray<FStorePrice> prices;

	UPROPERTY(VisibleAnywhere)
	FString image_url;

public:
	FStoreItem(){};
};

USTRUCT(BlueprintType)
struct FStoreItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FStoreItem> Items;

public:
	FStoreItemsData(){};
};
