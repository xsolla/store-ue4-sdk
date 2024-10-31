// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Variant.h"

#include "XsollaUtilsDataModel.generated.h"

DECLARE_DELEGATE_TwoParams(FOnTokenUpdate, const FString&, bool);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnError, int32, StatusCode, int32, ErrorCode, const FString&, ErrorMessage);

USTRUCT()
struct FErrorData
{
	GENERATED_BODY()

	int32 StatusCode = -1;
	int32 ErrorCode = -1;
	FString ErrorMessage;

	FErrorData(int32 InStatusCode, int32 InErrorCode, const FString& InErrorMessage)
		: StatusCode(InStatusCode)
		, ErrorCode(InErrorCode)
		, ErrorMessage(InErrorMessage)
	{
	}

	FErrorData()
	{
	}
};

USTRUCT()
struct FErrorHandlersWrapper
{
	GENERATED_BODY()

	bool bNeedRepeatRequest = false;
	FOnTokenUpdate TokenUpdateCallback;
	FOnError ErrorCallback;
	FErrorHandlersWrapper(bool bInNeedRepeatRequest, FOnTokenUpdate InTokenUpdateCallback, FOnError InErrorCallback)
		: bNeedRepeatRequest(bInNeedRepeatRequest)
		, TokenUpdateCallback(InTokenUpdateCallback)
		, ErrorCallback(InErrorCallback)
	{
	}
	FErrorHandlersWrapper(FOnError InErrorCallback)
		: ErrorCallback(InErrorCallback)
	{
	}
	FErrorHandlersWrapper() {}
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaItemGroup
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

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString parent_external_id;

public:
	FXsollaItemGroup()
		: id(-1)
		, level(0)
		, order(1){};
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaStoreItemDiscount
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Discount")
	FString percent;

	UPROPERTY(BlueprintReadOnly, Category = "Item Discount")
	FString value;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaStoreItemBonus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Bonus")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Item Bonus")
	int32 quantity = 0;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaStoreItemRecurrentSchedule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Recurrent Schedule")
	FString interval_type;

	UPROPERTY(BlueprintReadOnly, Category = "Item Recurrent Schedule")
	int32 reset_next_date = 0;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaStoreItemLimitsPerUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Limits Per User")
	int32 available = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Item Limits Per User")
	int32 total = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Item Limits Per User")
	FXsollaStoreItemRecurrentSchedule recurrent_schedule;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaStoreItemLimits
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Limits")
	FXsollaStoreItemLimitsPerUser per_user;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaPromotionRecurrentSchedule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Promotion Recurrent Schedule")
	FString interval_type;

	UPROPERTY(BlueprintReadOnly, Category = "Promotion Recurrent Schedule")
	FString time;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaPromotionLimitsPerUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Promotion Limits Per User")
	int32 available = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Promotion Limits Per User")
	int32 total = 0;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaPromotionLimits
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Promotion Limits")
	FXsollaPromotionLimitsPerUser per_user;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaStoreItemPromotion
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Promotion")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Item Promotion")
	FString date_start;

	UPROPERTY(BlueprintReadOnly, Category = "Item Promotion")
	FString date_end;

	UPROPERTY(BlueprintReadOnly, Category = "Item Promotion")
	FXsollaStoreItemDiscount discount;

	UPROPERTY(BlueprintReadOnly, Category = "Item Promotion")
	TArray<FXsollaStoreItemBonus> bonus;

	UPROPERTY(BlueprintReadOnly, Category = "Item Promotion")
	FXsollaPromotionLimits limits;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaItemAttributeValue
{
	GENERATED_BODY()

	/** Unique value ID for an attribute. The external_id may only contain lowercase Latin alphanumeric characters, dashes, and underscores. */
	UPROPERTY(BlueprintReadOnly, Category = "Item Attribute Value")
	FString external_id;

	/* Value of attribute. */
	UPROPERTY(BlueprintReadOnly, Category = "Item Attribute Value")
	FString value;
};

/** Attribute and their values corresponding to the item. Can be used for catalog filtering. */
USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaItemAttribute
{
	GENERATED_BODY()

	/** Unique attribute ID. The `external_id` may only contain lowercase Latin alphanumeric characters, dashes, and underscores. */
	UPROPERTY(BlueprintReadOnly, Category = "Item Attribute")
	FString external_id;

	/* Name off attribute. */
	UPROPERTY(BlueprintReadOnly, Category = "Item Attribute")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Item Attribute")
	TArray<FXsollaItemAttributeValue> values;

public:
	FXsollaItemAttribute() {}
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaPrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Price")
	FString amount;

	UPROPERTY(BlueprintReadOnly, Category = "Item Price")
	FString amount_without_discount;

	UPROPERTY(BlueprintReadOnly, Category = "Item Price")
	FString currency;

public:
	FXsollaPrice() {}
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaConsumable
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Consumable")
	int32 usages_count;

public:
	FXsollaConsumable()
		: usages_count(0)
	{
	}
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaExpirationPeriod
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Expiration Period")
	int32 value;

	UPROPERTY(BlueprintReadOnly, Category = "Item Expiration Period")
	FString type;

public:
	FXsollaExpirationPeriod()
		: value(0)
	{
	}
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaItemOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Options")
	FXsollaConsumable consumable;

	UPROPERTY(BlueprintReadOnly, Category = "Item Options")
	FXsollaExpirationPeriod expiration_period;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaVirtualCurrencyCalculatedPrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Calculated Price")
	FString amount;

	UPROPERTY(BlueprintReadOnly, Category = "Virtual Currency Calculated Price")
	FString amount_without_discount;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaVirtualCurrencyPrice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	bool is_default;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	int amount;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	int amount_without_discount;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Item Virtual Currency Price")
	FXsollaVirtualCurrencyCalculatedPrice calculated_price;

public:
	FXsollaVirtualCurrencyPrice()
		: is_default(false)
		, amount(0)
		, amount_without_discount(0)
	{
	}
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaUnitItem
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
struct XSOLLAUTILS_API FXsollaRewardItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	FString virtual_item_type;

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Reward Item")
	TArray<FXsollaUnitItem> unit_items;
};

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FXsollaBonusItem
{
public:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Bonus Item")
	FXsollaRewardItem item;

	UPROPERTY(BlueprintReadOnly, Category = "Bonus Item")
	int32 quantity;

public:
	FXsollaBonusItem()
		: quantity(0){};
};

USTRUCT(BlueprintType)
struct FXsollaJsonVariant
{
	GENERATED_BODY()

	FXsollaJsonVariant(){};
	FXsollaJsonVariant(const FVariant Variant)
		: Variant(Variant){};

	FVariant Variant;
};

USTRUCT(BlueprintType)
struct FXsollaParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Parameters")
	TMap<FString, FXsollaJsonVariant> Parameters;
};

/** Pay Station version. */
UENUM(BlueprintType)
enum class EXsollaPayStationVersion : uint8
{
	v3 UMETA(DisplayName = "V3"),
	v4 UMETA(DisplayName = "V4")
};

USTRUCT(BlueprintType)
struct FXsollaPaymentTokenRequestPayload
{
	GENERATED_BODY()

	/**
	 * Currency (optional) Desired payment currency (USD by default). Three-letter currency code per [ISO 4217](https://en.wikipedia.org/wiki/ISO_4217) . Check the documentation for detailed information about [currencies supported by Xsolla](https://developers.xsolla.com/doc/pay-station/references/supported-currencies/).
	 * Leave empty to use the default value.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	FString Currency;

	/**
	 * Locale (optional) Desired payment locale.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	FString Locale;

	/**
	 * Quantity Item quantity.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	int32 Quantity = 1;

	/**
	 * CustomParameters (optional) Map of custom parameters.
	 * Leave empty to use the default value.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	FXsollaParameters CustomParameters;

	/**
	 * ExternalId (optional) Transaction external ID.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	FString ExternalId;

	/**
	 * bShowCloseButton Whether to show the ← icon in Pay Station so the user can close the payment UI at any stage of the purchase.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	bool bShowCloseButton = false;

	/**
	 * CloseButtonIcon Defines the icon of the **Close** button in the payment UI. Can be `arrow` or `cross`. `cross` by default.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	FString CloseButtonIcon = TEXT("cross");

	/**
	 * PayStationVersion Pay Station version. V4 by default.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	EXsollaPayStationVersion PayStationVersion = EXsollaPayStationVersion::v4;

	/**
	 * bGpQuickPaymentButton Defines the way the Google Pay payment method is displayed. If true, the button for quick payment via Google Pay is displayed at the top of the payment UI. If `true`, Google Pay is displayed in the list of payment methods according to the PayRank algorithm. `false` by default.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Xsolla Payment Token Request Payload")
	bool bGpQuickPaymentButton = false;
};

/* Usual version EVariantTypes isn't using UENUM(). It causes the problem when calling EnumToString with "EVariantTypes" as the first argument.
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
