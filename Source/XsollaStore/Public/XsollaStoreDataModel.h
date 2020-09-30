// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Misc/Variant.h"
#include "XsollaStoreDefines.h"
#include "XsollaUtilsDataModel.h"

#include "XsollaStoreDataModel.generated.h"

UENUM(BlueprintType)
enum class EXsollaOrderStatus : uint8
{
	Unknown,
	New,
	Paid,
	Done
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Store Item Attribute")
	int32 stack_size;

	UPROPERTY(BlueprintReadOnly, Category = "Store Item Attribute")
	bool licensed;

public:
	FStoreItemAttribute()
		: stack_size(0)
		, licensed(false){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStorePrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	FString amount;

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	FString amount_without_discount;

	UPROPERTY(BlueprintReadOnly, Category = "Price")
	FString currency;

public:
	FStorePrice(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FItemExpirationPeriod
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Store Item Expiration Period")
	int32 value;

	UPROPERTY(BlueprintReadOnly, Category = "Store Item Expiration Period")
	FString type;

public:
	FItemExpirationPeriod()
		: value(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FItemInventoryOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Store Item Inventory Options")
	FItemExpirationPeriod expiration_period;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyCalculatedPrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Calculated Price")
	FString amount;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Calculated Price")
	FString amount_without_discount;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyPrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	bool is_default;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	int amount;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	int amount_without_discount;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Price")
	FVirtualCurrencyCalculatedPrice calculated_price;

public:
	FVirtualCurrencyPrice()
		: is_default(false)
		, amount(0)
		, amount_without_discount(0){};
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
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FStoreGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FStorePrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FItemInventoryOptions inventory_options;

public:
	FStoreItem()
		: is_free(false){};

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
		, inventory_options(Item.inventory_options){};

	bool operator==(const FStoreItem& Item) const
	{
		return sku == Item.sku;
	}
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
struct XSOLLASTORE_API FVirtualCurrency
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FString> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	int order;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	TArray<FString> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency")
	FStorePrice price;

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
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package Content")
	int quantity;

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
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	int order;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FStoreGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FStorePrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FVirtualCurrencyPackageContent content;

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

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Packages Data")
	TArray<FVirtualCurrencyPackage> Items;

public:
	FVirtualCurrencyPackagesData(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyBalance
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Virtual Currency Balance")
	FString sku;

	UPROPERTY(BlueprintReadWrite, Category = "Virtual Currency Balance")
	FString name;

	UPROPERTY(BlueprintReadWrite, Category = "Virtual Currency Balance")
	FString description;

	UPROPERTY(BlueprintReadWrite, Category = "Virtual Currency Balance")
	FString image_url;

	UPROPERTY(BlueprintReadWrite, Category = "Virtual Currency Balance")
	int64 amount;

public:
	FVirtualCurrencyBalance()
		: amount(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FVirtualCurrencyBalanceData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Balance Data")
	TArray<FVirtualCurrencyBalance> Items;

public:
	FVirtualCurrencyBalanceData(){};
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
	FStorePrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	TArray<FVirtualCurrencyPrice> vc_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	int32 quantity;

public:
	FStoreCartItem()
		: is_free(false)
		, quantity(0){};

	FStoreCartItem(const FStoreItem& Item)
		: sku(Item.sku)
		, name(Item.name)
		, is_free(Item.is_free)
		, image_url(Item.image_url)
		, quantity(0){};

	FStoreCartItem(const FVirtualCurrencyPackage& CurrencyPackage)
		: sku(CurrencyPackage.sku)
		, name(CurrencyPackage.name)
		, is_free(CurrencyPackage.is_free)
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
	FStorePrice price;

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
struct XSOLLASTORE_API FStoreInventoryItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	TArray<FString> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	TArray<FStoreGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString instance_id;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	int32 quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	int32 remaining_uses;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreInventory
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Data")
	TArray<FStoreInventoryItem> Items;

public:
	FStoreInventory(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreSubscriptionItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString Class;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	int64 expired_at;

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Item")
	FString status;

public:
	FStoreSubscriptionItem()
		: expired_at(0)
	{
	}
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreSubscriptionData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Data")
	TArray<FStoreSubscriptionItem> Items;

public:
	FStoreSubscriptionData(){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreUnitItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Unit Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Unit Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Unit Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Unit Item")
	FString drm_name;

	UPROPERTY(BlueprintReadOnly, Category = "Unit Item")
	FString drm_sku;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreCouponRewardItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	TArray<FStoreUnitItem> unit_items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreCoupoBonusItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	FStoreCouponRewardItem item;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	int32 quantity;

public:
	FStoreCoupoBonusItem()
		: quantity(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreCouponRewardData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	TArray<FStoreCoupoBonusItem> bonus;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	bool is_selectable;

public:
	FStoreCouponRewardData()
		: is_selectable(false){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreRedeemedCouponItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	TArray<FStoreGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	TArray<FStoreItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FStorePrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	TArray<FVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FItemInventoryOptions inventory_options;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	int32 quantity;

public:
	FStoreRedeemedCouponItem()
		: is_free(false)
		, quantity(0){};
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreRedeemedCouponData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Data")
	TArray<FStoreRedeemedCouponItem> items;
};

USTRUCT(BlueprintType)
struct FXsollaJsonVariant
{
	GENERATED_BODY()

	FXsollaJsonVariant(){};
	FXsollaJsonVariant(const FVariant Variant) : Variant(Variant){};

	FVariant Variant;
};

USTRUCT(BlueprintType)
struct FXsollaPaymentCustomParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FXsollaJsonVariant> Parameters;
};

/* Usual version EVariantTypes is not using UENUM() this cause problem when call GetEnumValueAsString with "EVariantTypes" as first argument.
*/
UENUM()
enum class EXsollaVariantTypes : int8
{
	Empty = 0,
	Ansichar = 1,
	Bool = 2,
	Box = 3,
	BoxSphereBounds = 4,
	ByteArray = 5,
	Color = 6,
	DateTime = 7,
	Double = 8,
	Enum = 9,
	Float = 10,
	Guid = 11,
	Int8 = 12,
	Int16 = 13,
	Int32 = 14,
	Int64 = 15,
	IntRect = 16,
	LinearColor = 17,
	Matrix = 18,
	Name = 19,
	Plane = 20,
	Quat = 21,
	RandomStream = 22,
	Rotator = 23,
	String = 24,
	Widechar = 25,
	Timespan = 26,
	Transform = 27,
	TwoVectors = 28,
	UInt8 = 29,
	UInt16 = 30,
	UInt32 = 31,
	UInt64 = 32,
	Vector = 33,
	Vector2d = 34,
	Vector4 = 35,
	IntPoint = 36,
	IntVector = 37,
	NetworkGUID = 38,
	Custom = 0x40
};