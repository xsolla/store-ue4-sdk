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
	int quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Bundle")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;
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
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item")
	FXsollaItemOptions inventory_options;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	FString bundle_type;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	FXsollaPrice total_content_price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	TArray<FStoreBundleContent> content;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Item Bundle")
	TArray<FXsollaItemAttribute> attributes;

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
		, inventory_options(Item.inventory_options)
		, bundle_type(Item.bundle_type)
		, total_content_price(Item.total_content_price)
		, content(Item.content)
		, attributes(Item.attributes)
	{
	}

	FStoreItem(const struct FStoreBundle& Bundle);

	bool operator==(const FStoreItem& Item) const
	{
		return sku == Item.sku;
	}
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Items List")
	TArray<FStoreItem> Items;
};

USTRUCT(BlueprintType)
struct XSOLLASTORE_API FStoreItemsData
{
public:
	GENERATED_BODY()

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
	FXsollaPrice price;

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
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Package")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

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
struct XSOLLASTORE_API FStoreCartItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Cart Item")
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
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	TArray<FXsollaVirtualCurrencyPrice> vc_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Cart Item")
	FString image_url;

	UPROPERTY(BlueprintReadWrite, Category = "Cart Item")
	int32 quantity;

public:
	FStoreCartItem()
		: is_free(false)
		, quantity(0){};

	FStoreCartItem(const FStoreItem& Item)
		: sku(Item.sku)
		, name(Item.name)
		, is_free(Item.is_free)
		, price(Item.price)
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
	bool is_selectable;
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

	UPROPERTY(BlueprintReadWrite, Category = "Payment Custom Parameters")
	TMap<FString, FXsollaJsonVariant> Parameters;
};

/* Usual version EVariantTypes isn't using UENUM(). It causes the problem when calling GetEnumValueAsString with "EVariantTypes" as the first argument.
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
