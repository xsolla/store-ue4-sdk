// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

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
	TArray<FXsollaItemAttribute> attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	TArray<FXsollaItemGroup> groups;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FString instance_id;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	int32 quantity = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	int32 remaining_uses = 0;
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

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Balance")
	FString type;

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
struct XSOLLAINVENTORY_API FTimeLimitedItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	int64 expired_at;

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item")
	FString status;

public:
	FTimeLimitedItem()
		: expired_at(0)
	{
	}
};

USTRUCT(BlueprintType)
struct XSOLLAINVENTORY_API FTimeLimitedItemsData
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Time Limited Item Data")
	TArray<FTimeLimitedItem> Items;

public:
	FTimeLimitedItemsData(){};
};