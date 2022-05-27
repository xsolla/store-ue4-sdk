// Copyright 2021 Xsolla Inc. All Rights Reserved.

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
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSubscriptionUpdate, const FSubscriptionData&, SubscriptionData);
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
	 * @param InProjectId New Project ID value from Publisher Account Project settings > Project ID.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory")
	void Initialize(const FString& InProjectId);

	/** Get List of Purchased Virtual Items
	 * Gets the list of purchased virtual items.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Target platform
	 * @param SuccessCallback Callback function called after purchased virtual items were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetInventory(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
		const FOnInventoryUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Virtual Currency Balance
	 * Gets virtual currency balance.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Target platform
	 * @param SuccessCallback Callback function called after virtual currency balance was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyBalance(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
		const FOnCurrencyBalanceUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get User Subscriptions
	 * Gets the list of user subscriptions.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Target platform
	 * @param SuccessCallback Callback function called after list of user subscriptions was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptions(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
		const FOnSubscriptionUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Consume Inventory Item
	 * Consumes an inventory item.
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Item quantity. If the item is uncountable, should be zero.
	 * @param InstanceID Instance item ID. If the item is countable, should be empty.
	 * @param Platform Target platform
	 * @param SuccessCallback Callback function called after successful inventory item consumption.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU, const int32 Quantity,
		const FString& InstanceID, const EXsollaPublishingPlatform Platform,
		const FOnInventoryRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Coupon Rewards
	 * Gets coupon rewards by its code. Can be used to let users choose one of many items as a bonus.
	 * The usual case is choosing a DRM if the coupon contains a game as a bonus.
	 *
	 * @param AuthToken User authorization token.
	 * @param CouponCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Callback function called after receiving coupon rewards successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|Coupons", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetCouponRewards(const FString& AuthToken, const FString& CouponCode,
		const FOnCouponRewardsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Redeem Coupon
	 * Redeems a coupon code. The user gets a bonus after a coupon is redeemed.
	 *
	 * @param AuthToken User authorization token.
	 * @param CouponCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Callback function called after successful coupon redemption.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|Coupons", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemCoupon(const FString& AuthToken, const FString& CouponCode,
		const FOnCouponRedeemUpdate& SuccessCallback, const FOnError& ErrorCallback);

protected:
	void GetInventory_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetVirtualCurrencyBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCurrencyBalanceUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetSubscriptions_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnSubscriptionUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

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
