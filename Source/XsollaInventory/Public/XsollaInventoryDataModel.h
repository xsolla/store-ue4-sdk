// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Misc/Variant.h"
#include "XsollaInventoryDefines.h"
#include "XsollaUtilsDataModel.h"

#include "XsollaInventoryDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FInventoryItem
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
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString instance_id;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	int32 quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	int32 remaining_uses;
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FInventoryItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Data")
	TArray<FInventoryItem> Items;

public:
	FInventoryItemsData(){};
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FInventoryCouponRewardData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	TArray<FXsollaBonusItem> bonus;

	UPROPERTY(BlueprintReadOnly, Category = "Coupon Data")
	bool is_selectable;

public:
	FInventoryCouponRewardData()
		: is_selectable(false){};
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FInventoryRedeemedCouponItem
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
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	bool is_free;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FXsollaPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	TArray<FXsollaVirtualCurrencyPrice> virtual_prices;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	FXsollaItemOptions inventory_options;

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Item")
	int32 quantity;

public:
	FInventoryRedeemedCouponItem()
		: is_free(false)
		, quantity(0){};
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FInventoryRedeemedCouponData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Redeemed Coupon Data")
	TArray<FInventoryRedeemedCouponItem> items;
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FVirtualCurrencyBalance
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
struct XSOLLAINVENTORY_API FVirtualCurrencyBalanceData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Balance Data")
	TArray<FVirtualCurrencyBalance> Items;

public:
	FVirtualCurrencyBalanceData(){};
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FSubscriptionItem
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
	FSubscriptionItem()
		: expired_at(0)
	{
	}
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FSubscriptionData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subscription Data")
	TArray<FSubscriptionItem> Items;

public:
	FSubscriptionData(){};
};