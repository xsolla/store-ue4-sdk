// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaInventoryDataModel.h"

#include "XsollaUtilsHttpRequestHelper.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "XsollaUtilsDataModel.h"

#include "XsollaInventorySubsystem.generated.h"

enum class EXsollaPublishingPlatform : uint8;
class FJsonObject;
class UXsollaLoginSubsystem;

DECLARE_DYNAMIC_DELEGATE(FOnInventoryRequestSuccess);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInventoryUpdate, const FInventoryItemsData&, InventoryData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCurrencyBalanceUpdate, const FVirtualCurrencyBalanceData&, CurrencyBalanceData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTimeLimitedItemsUpdate, const FTimeLimitedItemsData&, TimeLimitedItemsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCouponRewardsUpdate, FInventoryCouponRewardData, RewardsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCouponRedeemUpdate, FInventoryRedeemedCouponData, RewardData);

UCLASS()
class XSOLLAINVENTORY_API UXsollaInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UXsollaInventorySubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/**
	 * Initialize the controller with provided Project ID (use to override project settings)
	 *
	 * @param InProjectId New Project ID value from Publisher Account. It can be found in Publisher Account next to the name of your project.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory")
	void Initialize(const FString& InProjectId);

	/** Returns a list of virtual items from the user’s inventory according to pagination settings. For each virtual item, complete data is returned.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/player-inventory/display-inventory/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param Platform Publishing platform the user plays on.<br>
	 * Can be `xsolla` (default), `playstation_network`, `xbox_live`, `pc_standalone`, `nintendo_shop`, `google_play`, `app_store_ios`, `android_standalone`, `ios_standalone`, `android_other`, `ios_other`, or `pc_other`.
	 * @param SuccessCallback Called after purchased virtual items were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page. The maximum number of elements on a page is 50.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetInventory(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
		const FOnInventoryUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns the current user’s balance of virtual currency. For each virtual currency, complete data is returned.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/player-inventory/display-inventory/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param Platform Publishing platform the user plays on.<br>
	 * Can be `xsolla` (default), `playstation_network`, `xbox_live`, `pc_standalone`, `nintendo_shop`, `google_play`, `app_store_ios`, `android_standalone`, `ios_standalone`, `android_other`, `ios_other`, or `pc_other`.
	 * @param SuccessCallback Called after virtual currency balance was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyBalance(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
		const FOnCurrencyBalanceUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of time-limited items from the user’s inventory. For each item, complete data is returned.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/player-inventory/display-inventory/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param Platform Publishing platform the user plays on.<br>
	 * Can be `xsolla` (default), `playstation_network`, `xbox_live`, `pc_standalone`, `nintendo_shop`, `google_play`, `app_store_ios`, `android_standalone`, `ios_standalone`, `android_other`, `ios_other`, or `pc_other`.
	 * @param SuccessCallback Called after list of user time limited items was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|TimeLimitedItems", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetTimeLimitedItems(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
		const FOnTimeLimitedItemsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Consumes an inventory item. Use for only for consumable virtual items.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/player-inventory/consume-item/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Item quantity. If the item is uncountable, should be zero.
	 * @param InstanceID Instance item ID. If the item is countable, should be empty.
	 * @param Platform Publishing platform the user plays on.<br>
	 * Can be `xsolla` (default), `playstation_network`, `xbox_live`, `pc_standalone`, `nintendo_shop`, `google_play`, `app_store_ios`, `android_standalone`, `ios_standalone`, `android_other`, `ios_other`, or `pc_other`.
	 * @param SuccessCallback Called after successful inventory item consumption.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU, const int32 Quantity,
		const FString& InstanceID, const EXsollaPublishingPlatform Platform,
		const FOnInventoryRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of items that can be credited to the user when the coupon is redeemed. Can be used to let users choose one of many items as a bonus.
	 * The usual case is choosing a DRM if the coupon contains a game as a bonus.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/coupons).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CouponCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Called after receiving coupon rewards successfully.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|Coupons", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetCouponRewards(const FString& AuthToken, const FString& CouponCode,
		const FOnCouponRewardsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Redeems the coupon code and delivers a reward to the user in one of the following ways:
	 * - to their inventory (virtual items, virtual currency packages, or bundles)
	 * - via email (game keys)
	 * - to the entitlement system (game keys)<br>
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/coupons).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CouponCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Called after successful coupon redemption.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|Coupons", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemCoupon(const FString& AuthToken, const FString& CouponCode,
		const FOnCouponRedeemUpdate& SuccessCallback, const FOnError& ErrorCallback);

protected:
	void GetInventory_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetVirtualCurrencyBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCurrencyBalanceUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetTimeLimitedItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnTimeLimitedItemsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void ConsumeInventoryItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnInventoryRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void UpdateCouponRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCouponRewardsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void RedeemCoupon_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCouponRedeemUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET,
		const FString& AuthToken = FString(), const FString& Content = FString());

	/** Serialize json object into string */
	FString SerializeJson(const TSharedPtr<FJsonObject> DataJson) const;

	UPROPERTY()
	UXsollaLoginSubsystem* LoginSubsystem;

protected:
	/** Cached Xsolla Store project id */
	FString ProjectID;
};
