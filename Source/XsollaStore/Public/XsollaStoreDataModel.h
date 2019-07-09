// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDefines.h"

#include "XsollaStoreDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStorePrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	float amount;

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	FString currency;

public:
	FStorePrice()
		: amount(0.f){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreCartPrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	float amount;

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	float amount_without_discount;

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	FString currency;

public:
	FStoreCartPrice()
		: amount(0.f)
		, amount_without_discount(0.f){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString long_description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FString> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FStorePrice> prices;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString image_url;

public:
	FStoreItem()
		: is_free(false){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TArray<FStoreItem> Items;

public:
	FStoreItemsData(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreCart
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	int32 cart_id;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	FStoreCartPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	TArray<FStoreItem> Items;

public:
	FStoreCart()
		: cart_id(INVALID_CART)
		, is_free(false){};

	FStoreCart(int32 CartId)
		: cart_id(CartId)
		, is_free(false){};
};
