// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDefines.h"

#include "XsollaStoreDataModel.generated.h"

UENUM(BlueprintType)
enum class EXsollaOrderStatus : uint8
{
	Unknown,
	New,
	Paid
};

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

	/** TNumericLimits<int32>::Max() if no limit */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	int32 purchase_limit;

public:
	FStoreItem()
		: is_free(false)
		, purchase_limit(TNumericLimits<int32>::Max()){};

	FStoreItem(const FStoreItem& Item)
		: sku(Item.sku)
		, name(Item.name)
		, description(Item.description)
		, long_description(Item.long_description)
		, groups(Item.groups)
		, is_free(Item.is_free)
		, prices(Item.prices)
		, image_url(Item.image_url)
		, purchase_limit(Item.purchase_limit){};

	bool operator==(const FStoreItem& Item) const
	{
		return sku == Item.sku;
	}
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	int32 id;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString external_id;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	int32 level;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	int32 order;

	// https://github.com/xsolla/store-ue4-sdk/issues/103
	//UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	//TArray<int32> children;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString parent_external_id;

public:
	FStoreGroup()
		: id(-1)
		, level(0)
		, order(1){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TArray<FStoreItem> Items;

	/** All category ids that used by items (calculated locally!) */
	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TSet<FString> GroupIds;

	/** All available item group infos */
	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TArray<FStoreGroup> Groups;

public:
	FStoreItemsData(){};
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
struct XSOLLASTORE_API FStoreCartItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString long_description;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FStoreCartPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	int32 quantity;

	/** TNumericLimits<int32>::Max() if no limit */
	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	int32 purchase_limit;

public:
	FStoreCartItem()
		: is_free(false)
		, quantity(0)
		, purchase_limit(TNumericLimits<int32>::Max()){};

	FStoreCartItem(const FStoreItem& Item)
		: sku(Item.sku)
		, name(Item.name)
		, is_free(Item.is_free)
		, image_url(Item.image_url)
		, quantity(0)
		, purchase_limit(Item.purchase_limit){};

	bool operator==(const FStoreCartItem& Item) const
	{
		return sku == Item.sku;
	}

	/*bool operator==(const FStoreItem& Item) const
	{
		return sku == Item.sku;
	}*/
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
	TArray<FStoreCartItem> Items;

public:
	FStoreCart()
		: cart_id(INVALID_CART)
		, is_free(false){};

	FStoreCart(int32 CartId)
		: cart_id(CartId)
		, is_free(false){};

	// Check we have the same set if items
	bool operator==(const FStoreCart& Cart) const
	{
		if (cart_id == Cart.cart_id)
		{
			if (Items.Num() == Cart.Items.Num())
			{
				int32 ItemsCount = Items.Num();
				for (int32 i = 0; i < ItemsCount; ++i)
				{
					if (Items[i].sku == Cart.Items[i].sku)
					{
						if (Items[i].quantity != Cart.Items[i].quantity)
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}

				return true;
			}
		}

		return false;
	}
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreInventory
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Data")
	TArray<FStoreCartItem> Items;

public:
	FStoreInventory(){};
};
