// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaUtilsDataModel.h"
#include "XsollaStoreDataModel.generated.h"

UENUM(BlueprintType)
enum class EXsollaOrderStatus : uint8
{
	Unknown,
	New,
	Paid,
	Done,
	Canceled
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FXsollaOrderItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Order Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Order Item")
	int32 quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Order Item")
	FString is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Order Item")
	FXsollaPrice price;

	FXsollaOrderItem()
		: quantity(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FXsollaOrderContent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Order Content")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Order Content")
	FXsollaVirtualCurrencyPrice virtual_price;

	UPROPERTY(BlueprintReadOnly, Category = "Order Content")
	FString is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Order Content")
	TArray<FXsollaOrderItem> items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FXsollaOrder
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Order")
	int32 order_id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Order")
	FString status;

	UPROPERTY(BlueprintReadOnly, Category = "Order")
	FXsollaOrderContent content;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreBundleContent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	int32 quantity = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemMediaList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Media List")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Media List")
	FString url;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItem
{
	GENERATED_BODY()

	/* Unique item ID. The SKU may only contain lowercase Latin alphanumeric characters, periods, dashes, and underscores. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString sku;

	/* Item name. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString name;

	/* Item description. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString description;

	/* Type of item: Consumable/Expiration/Permanent/Lootboxes/Physical. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString type;

	/* Type of virtual item. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString virtual_item_type;

	/* Groups the item belongs to. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FXsollaItemGroup> groups;

	/* If `true`, the item is free. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	bool is_free;

	/* Item prices */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FXsollaPrice price;

	/* Virtual prices. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	/* Image URL. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString image_url;

	/* Defines the inventory item options. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FXsollaItemOptions inventory_options;

	/* Type off bundle. In this case, it is always `standart`. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	FString bundle_type;

	/* Sum of the bundle content prices. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	FXsollaPrice total_content_price;

	/* Bundle package content. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	TArray<FStoreBundleContent> content;

	/* List of attributes and their values corresponding to the item. Can be used for catalog filtering. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	TArray<FXsollaItemAttribute> attributes;

	/* Item long description. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString long_description;

	/* Value that defines arrangement order. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	int32 order;

	/* List of additional item assets such as screenshots, gameplay video etc.. */
	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FStoreItemMediaList> media_list;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FXsollaStoreItemPromotion> promotions;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FXsollaStoreItemLimits limits;

public:
	FStoreItem()
		: is_free(false)
		, order(0){};

	FStoreItem(const FStoreItem& Item)
		: sku(Item.sku)
		, name(Item.name)
		, description(Item.description)
		, type(Item.type)
		, virtual_item_type(Item.virtual_item_type)
		, groups(Item.groups)
		, is_free(Item.is_free)
		, price(Item.price)
		, virtual_prices(Item.virtual_prices)
		, image_url(Item.image_url)
		, inventory_options(Item.inventory_options)
		, bundle_type(Item.bundle_type)
		, total_content_price(Item.total_content_price)
		, content(Item.content)
		, attributes(Item.attributes)
		, long_description(Item.long_description)
		, order(Item.order)
		, media_list(Item.media_list)
		, promotions(Item.promotions)
		, limits(Item.limits)
	{
	}

	FStoreItem(const struct FStoreBundle& Bundle);

	bool operator==(const FStoreItem& Item) const
	{
		return sku == Item.sku;
	}
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FXsollaStoreCustomAttributes
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Custom Attributes")
	int32 purchased;

	UPROPERTY(BlueprintReadOnly, Category = "Custom Attributes")
	FString attr;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreVpReward
{
	GENERATED_BODY()

	//TEXTREVIEW
	/* Unique item ID. */
	UPROPERTY(BlueprintReadOnly, Category = "Vp Reward")
	int32 item_id;

	/* Unique item ID. */
	UPROPERTY(BlueprintReadOnly, Category = "Vp Reward")
	FString sku;

	/* Amount. */
	UPROPERTY(BlueprintReadOnly, Category = "Vp Reward")
	int32 amount;

	/* Item name. */
	UPROPERTY(BlueprintReadOnly, Category = "Vp Reward")
	FString name;

	/* Image URL. */
	UPROPERTY(BlueprintReadOnly, Category = "Vp Reward")
	FString image_url;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSellableItem
{
	GENERATED_BODY()

	// TEXTREVIEW
	/* Unique item ID. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	int32 item_id;

	/* Unique item ID. The SKU may only contain lowercase Latin alphanumeric characters, periods, dashes, and underscores. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FString sku;

	/* Type of item: Consumable/Expiration/Permanent/Lootboxes/Physical. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FString type;

	/* Item name. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FString name;

	/* Item description. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FString description;

	/* Image URL. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FString image_url;

	/* Item prices */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FXsollaPrice price;

	/* Virtual prices. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	// TEXTREVIEW
	/* If `true`, the item can be bought. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	bool can_be_bought;

	/* Promotions. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	TArray<FXsollaStoreItemPromotion> promotions;

	/* List of attributes and their values corresponding to the item. Can be used for catalog filtering. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item Bundle")
	TArray<FXsollaItemAttribute> attributes;

	/* If `true`, the item is free. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	bool is_free;

	/* Groups the item belongs to. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	TArray<FXsollaItemGroup> groups;

	/* Defines the inventory item options. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FXsollaItemOptions inventory_options;

	/* Limits. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FXsollaStoreItemLimits limits;

	/* Custom attributes. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	FXsollaStoreCustomAttributes custom_attributes;

	// TEXTREVIEW
	/* VP rewards. */
	UPROPERTY(BlueprintReadOnly, Category = "Sellable Item")
	TArray<FStoreVpReward> vp_rewards;

public:
	FSellableItem()
		: item_id(0)
		, can_be_bought(false)
		, is_free(false){};

	bool operator==(const FSellableItem& Item) const
	{
		return sku == Item.sku;
	}
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Items List", meta = (DisplayName = "bHasMore"))
	bool has_more = false;

	UPROPERTY(BlueprintReadOnly, Category = "Items List")
	TArray<FStoreItem> Items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Items Data", meta = (DisplayName = "bHasMore"))
	bool has_more = false;

	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TArray<FStoreItem> Items;

	/** All category IDs used by items (calculated locally!). */
	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TSet<FString> GroupIds;

	/** All available item groups infos. */
	UPROPERTY(BlueprintReadOnly, Category = "Items Data")
	TArray<FXsollaItemGroup> Groups;

public:
	FStoreItemsData(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSellableItemsList
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Sellable Items List", meta = (DisplayName = "bHasMore"))
	bool has_more = false;

	UPROPERTY(BlueprintReadOnly, Category = "Sellable Items List")
	TArray<FSellableItem> Items;

public:
	FSellableItemsList(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrency
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FString> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FXsollaItemOptions inventory_options;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString long_description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	int32 order;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FStoreItemMediaList> media_list;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FXsollaStoreItemPromotion> promotions;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FXsollaStoreItemLimits limits;

public:
	FVirtualCurrency()
		: is_free(false)
		, order(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Data", meta = (DisplayName = "bHasMore"))
	bool has_more = false;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Data")
	TArray<FVirtualCurrency> Items;

public:
	FVirtualCurrencyData(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyPackageContent
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	int32 quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FXsollaItemOptions inventory_options;

public:
	FVirtualCurrencyPackageContent()
		: quantity(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyPackage
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString bundle_type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FVirtualCurrencyPackageContent content;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString long_description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	int32 order;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FStoreItemMediaList> media_list;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FXsollaStoreItemPromotion> promotions;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FXsollaStoreItemLimits limits;

public:
	FVirtualCurrencyPackage()
		: is_free(false)
		, order(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyPackagesData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Packages Data", meta = (DisplayName = "bHasMore"))
	bool has_more = false;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Packages Data")
	TArray<FVirtualCurrencyPackage> Items;

public:
	FVirtualCurrencyPackagesData(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreCartItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Cart Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString long_description;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	bool is_bonus;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	TArray<FXsollaVirtualCurrencyPrice> vc_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString image_url;

	UPROPERTY(BlueprintReadWrite, Category = "Cart Item")
	int32 quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FXsollaItemOptions inventory_options;

public:
	FStoreCartItem()
		: is_free(false)
		, is_bonus(false)
		, quantity(0){};

	FStoreCartItem(const FStoreItem& Item)
		: sku(Item.sku)
		, name(Item.name)
		, is_free(Item.is_free)
		, is_bonus(false)
		, price(Item.price)
		, image_url(Item.image_url)
		, quantity(0){};

	FStoreCartItem(const FVirtualCurrencyPackage& CurrencyPackage)
		: sku(CurrencyPackage.sku)
		, name(CurrencyPackage.name)
		, is_free(CurrencyPackage.is_free)
		, is_bonus(false)
		, price(CurrencyPackage.price)
		, image_url(CurrencyPackage.image_url)
		, quantity(0){};

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
	FString cart_id;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Data")
	TArray<FStoreCartItem> Items;

public:
	FStoreCart()
		: is_free(false){};

	FStoreCart(FString CartId)
		: cart_id(CartId)
		, is_free(false){};

	// Check if we have the same set of items.
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
struct XSOLLASTORE_API FStoreBundle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString bundle_type;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FString is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FXsollaPrice total_content_price;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FStoreBundleContent> content;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FXsollaStoreItemPromotion> promotions;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FXsollaStoreItemLimits limits;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreListOfBundles
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FStoreBundle> items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreDiscount
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Discount")
	FString percent;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStorePromocodeRewardData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Promocode Reward Data")
	TArray<FXsollaBonusItem> bonus;

	/** Percent discount. The price of the cart will be decreased by a value calculated by using this percent and then rounded to 2 decimal places. */
	UPROPERTY(BlueprintReadOnly, Category = "Promocode Reward Data")
	FStoreDiscount discount;

	/** If 'true', the user should choose the bonus before redeeming a promo code. */
	UPROPERTY(BlueprintReadOnly, Category = "Promocode Reward Data")
	bool is_selectable = false;
};

inline FStoreItem::FStoreItem(const struct FStoreBundle& Bundle)
{
	this->description = Bundle.description;
	this->groups = Bundle.groups;
	this->image_url = Bundle.image_url;
	this->is_free = Bundle.is_free == "true";
	this->name = Bundle.name;
	this->price = Bundle.price;
	this->sku = Bundle.sku;
	this->type = Bundle.type;
	this->virtual_prices = Bundle.virtual_prices;
	this->bundle_type = Bundle.bundle_type;
	this->total_content_price = Bundle.total_content_price;
	this->content = Bundle.content;
	this->attributes = Bundle.attributes;
}

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FGameUnitItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	bool is_free = false;
  
	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	FString drm_name;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	FString drm_sku;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	bool has_keys = false;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	bool is_pre_order = false;

	UPROPERTY(BlueprintReadOnly, Category = "Game Unit Item")
	FString release_date;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FGameKeyItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString name;
	
	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString type;

	/* Groups the item belongs to. */
	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString image_url;
	
	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	bool is_free = false;
  
	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString drm_name;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString drm_sku;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	bool has_keys = false;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	bool is_pre_order = false;

	UPROPERTY(BlueprintReadOnly, Category = "Game Key Item")
	FString release_date;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FGameItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	FString unit_type;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Game Item")
	TArray<FGameUnitItem> unit_items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreGamesList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game List")
	TArray<FGameItem> Games;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreGamesData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Games Data")
	TArray<FGameItem> Items;
	
	/** All category IDs used by games (calculated locally!). */
	UPROPERTY(BlueprintReadOnly, Category = "Games Data")
	TSet<FString> GroupIds;

	/** All available game groups infos. */
	UPROPERTY(BlueprintReadOnly, Category = "Games Data")
	TArray<FXsollaItemGroup> Groups;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreGameKeysList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Key List")
	TArray<FGameKeyItem> GameKeys;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FDRMItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "DRM Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "DRM Item")
	FString name;
	
	UPROPERTY(BlueprintReadOnly, Category = "DRM Item")
	FString image;

	UPROPERTY(BlueprintReadOnly, Category = "DRM Item")
	FString link;

	UPROPERTY(BlueprintReadOnly, Category = "DRM Item")
	FString redeem_instruction_link;

	UPROPERTY(BlueprintReadOnly, Category = "DRM Item")
	int32 drm_id = 0;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreDRMList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "DRM List")
	TArray<FDRMItem> drm;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FOwnedGameItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	FString name;
	
	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	int32 project_id = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	FString game_sku;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	FString drm;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	bool is_pre_order = false;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Game Item")
	TArray<FXsollaItemAttribute> attributes;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FOwnedGamesList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Owned Games List")
	bool has_more = false;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Games List")
	int32 total_items_count = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Owned Games List")
	TArray<FOwnedGameItem> items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionPaystationLink
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Paystation Link")
	FString link_to_ps;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionPeriod
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription Period")
	int32 value = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Period")
	FString unit;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionCharge
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription Charge")
	float amount = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Charge")
	float amount_with_promotion = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Charge")
	FString currency;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionPlanCharge
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan Charge")
	float amount = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan Charge")
	float setup_fee = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan Charge")
	FString currency;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionPlanPromotion
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan Promotion")
	float promotion_charge_amount = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan Promotion")
	float promotion_remaining_charges = 0.f;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionPlan
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	int32 plan_id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FString plan_external_id;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FString plan_group_id;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FString plan_type;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FString plan_name;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FString plan_description;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FDateTime plan_start_date;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FDateTime plan_end_date;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	int32 trial_period = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FSubscriptionPeriod period;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FSubscriptionPlanCharge charge;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plan")
	FSubscriptionPlanPromotion promotion;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionPlansList
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plans List")
	TArray<FSubscriptionPlan> items;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Plans List")
	bool has_more = false;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscription
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	int32 id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	int32 plan_id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString plan_external_id;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString plan_name;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString plan_description;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FDateTime plan_start_date;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FDateTime plan_end_date;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	int32 product_id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString product_external_id;
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString product_name;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString product_description;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FString status;
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	bool is_in_trial = false;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	int32 trial_period = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FDateTime date_create;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FDateTime date_next_charge;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FDateTime date_last_charge;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FSubscriptionCharge charge;
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscription")
	FSubscriptionPeriod period;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionsList
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Subscriptions List")
	TArray<FSubscription> items;

	UPROPERTY(BlueprintReadOnly, Category = "Subscriptions List")
	bool has_more = false;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FSubscriptionDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	int32 id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	int32 plan_id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString plan_external_id;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString plan_name;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString plan_description;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FDateTime plan_start_date;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FDateTime plan_end_date;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	int32 product_id = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString product_external_id;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString product_name;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString product_description;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FString status;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	bool is_in_trial = false;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	int32 trial_period = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FDateTime date_create;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FDateTime date_next_charge;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FDateTime date_last_charge;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FSubscriptionCharge charge;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FSubscriptionPeriod period;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	FDateTime date_end;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	bool is_renew_possible = false;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	bool is_change_to_non_renew_possible = false;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Details")
	bool is_change_plan_allowed = false;
};