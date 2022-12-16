// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"
#include "XsollaUtilsHttpRequestHelper.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "XsollaOrderCheckObject.h"
#include "XsollaUtilsDataModel.h"
#include "XsollaStoreSubsystem.generated.h"


enum class EXsollaPublishingPlatform : uint8;
class FJsonObject;
class UXsollaStoreBrowserWrapper;
class UXsollaLoginSubsystem;

DECLARE_DYNAMIC_DELEGATE(FOnStoreUpdate);
DECLARE_DYNAMIC_DELEGATE(FOnStoreSuccessPayment);
DECLARE_DYNAMIC_DELEGATE(FOnStoreCancelPayment);
DECLARE_DYNAMIC_DELEGATE(FOnStoreCartUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCartUpdate, const FStoreCart&, Cart);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchTokenSuccess, const FString&, AccessToken, int32, OrderId);
DECLARE_DELEGATE_ThreeParams(FOnCheckOrder, int32, EXsollaOrderStatus, FXsollaOrderContent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCurrencyUpdate, const FVirtualCurrency&, Currency);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCurrencyPackageUpdate, const FVirtualCurrencyPackage&, CurrencyPackage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPurchaseUpdate, int32, OrderId);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetPromocodeRewardsUpdate, FStorePromocodeRewardData, RewardsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetListOfBundlesUpdate, FStoreListOfBundles, ListOfBundles);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSpecifiedBundleUpdate, FStoreBundle, Bundle);
DECLARE_DYNAMIC_DELEGATE(FOnPromocodeUpdate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetItemsListBySpecifiedGroup, FStoreItemsList, ItemsList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetItemsList, FStoreItemsList, ItemsList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetGamesListBySpecifiedGroup, FStoreGamesList, GamesList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGameUpdate, const FGameItem&, Game);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGameKeyUpdate, const FGameKeyItem&, GameKey);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetGameKeysListBySpecifiedGroup, FStoreGameKeysList, GameKeysList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDRMListUpdate, FStoreDRMList, DRMList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnOwnedGamesListUpdate, FOwnedGamesList, GamesList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVirtualCurrenciesUpdate, const FVirtualCurrencyData&, VirtualCurrencyData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreGamesUpdate, const FStoreGamesData&, GamesData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreItemsUpdate, const FStoreItemsData&, ItemsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVirtualCurrencyPackagesUpdate, const FVirtualCurrencyPackagesData&, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnItemGroupsUpdate, const TArray<FXsollaItemGroup>&, ItemGroups);
DECLARE_DYNAMIC_DELEGATE(FOnRedeemGameCodeSuccess);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSubscriptionPublicPlansListUpdate, FSubscriptionPlansList, SubscriptionPlansList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSubscriptionPlansListUpdate, FSubscriptionPlansList, SubscriptionPlansList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSubscriptionsListUpdate, FSubscriptionsList, SubscriptionsList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSubscriptionDetailsSuccess, const FSubscriptionDetails&, SubscriptionDetails);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSubscriptionPayStationLinkSuccess, const FString&, LinkToPaystation);
DECLARE_DYNAMIC_DELEGATE(FOnCancelSubscriptionSuccess);

UCLASS()
class XSOLLASTORE_API UXsollaStoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UXsollaStoreSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/**
	 * Initializes the controller with the provided Project ID (used to override project settings).
	 *
	 * @param InProjectId New Project ID value from Publisher Account > Project settings > Project ID.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void Initialize(const FString& InProjectId);

	/** Get Virtual Items
	 * Gets the list of virtual items available for the configured project.
	 *
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields 'media_list', 'order' and 'long_description'.
	 * @param SuccessCallback Callback function called after virtual items were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param AuthToken User JWT obtained during authorization using Xsolla Login ([Bearer token](https://developers.xsolla.com/api/login/overview/#section/Authentication/Getting-a-user-token)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualItems(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnStoreItemsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Get Item Groups
	 * Gets the list of virtual item groups.
	 *
	 * @param Locale (optional) Response language (e.g. item name, item description). Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after virtual item groups were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetItemGroups(const FString& Locale,
		const FOnItemGroupsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Virtual Currencies
	 * Gets the list of virtual currencies.
	 *
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value. Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address in not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields 'media_list', 'order' and 'long_description'.
	 * @param SuccessCallback Callback function called after virtual currencies were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencies(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnVirtualCurrenciesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Virtual Currency Packages
	 * Gets the list of virtual currency packages.
	 *
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after virtual currency packages were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param AuthToken User JWT obtained during authorization using Xsolla Login ([Bearer token](https://developers.xsolla.com/api/login/overview/#section/Authentication/Getting-a-user-token)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyPackages(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnVirtualCurrencyPackagesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Get Items List By Specified Group
	 * Gets an item list from the specified group for building a catalog.
	 *
	 * @param ExternalId Group external ID.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after server response.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count start from 0).
	 * @param AuthToken User JWT obtained during authorization using Xsolla Login ([Bearer token](https://developers.xsolla.com/api/login/overview/#section/Authentication/Getting-a-user-token)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualItems", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetItemsListBySpecifiedGroup(const FString& ExternalId, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetItemsListBySpecifiedGroup& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Get All Items List
	 * Gets a list of all virtual items.
	 *
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after server response.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param AuthToken User JWT obtained during authorization using Xsolla Login ([Bearer token](https://developers.xsolla.com/api/login/overview/#section/Authentication/Getting-a-user-token)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualItems", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetAllItemsList(const FString& Locale, const FOnGetItemsList& SuccessCallback, const FOnError& ErrorCallback, const FString& AuthToken = TEXT(""));

	/** Fetch Payment Token
	 * Initiates an item purchase session and fetches token for payment console.
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param Currency (optional) Desired payment currency. Leave empty to use the default value.
	 * @param Country (optional) Desired payment country ISO code. Leave empty to use the default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use the default value.
	 * @param CustomParameters (optional) Map of custom parameters. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after payment token was successfully fetched.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Quantity Item quantity.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU,
		const FString& Currency, const FString& Country, const FString& Locale,
		const FXsollaParameters CustomParameters,
		const FOnFetchTokenSuccess& SuccessCallback, const FOnError& ErrorCallback, const int32 Quantity = 1);

	/** Fetch Cart Payment Token
	 * Initiates a cart purchase session and fetches a token for payment console.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of the cart for the purchase. The current user cart will be purchased if empty.
	 * @param Currency (optional) Desired payment currency. Leave empty to use the default value.
	 * @param Country (optional) Desired payment country ISO code. Leave empty to use the default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use the default value.
	 * @param CustomParameters (optional) Map of custom parameters. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after the payment token was successfully fetched.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchCartPaymentToken(const FString& AuthToken, const FString& CartId,
		const FString& Currency, const FString& Country, const FString& Locale,
		const FXsollaParameters CustomParameters,
		const FOnFetchTokenSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Launch Payment Console
	 * Opens payment console for the provided access token.
	 *
	 * @param WorldContextObject The world context.
	 * @param OrderId Identifier of order.
	 * @param AccessToken Payment token used during purchase processing.
	 * @param SuccessCallback Callback function called after the payment was successfully completed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LaunchPaymentConsole(UObject* WorldContextObject, const int32 OrderId, const FString& AccessToken,
		const FOnStoreSuccessPayment& SuccessCallback, const FOnError& ErrorCallback);

	/** Check Pending Order
	 * Checks pending order
	 *
	 * @param AccessToken Payment token used during purchase processing.
	 * @param OrderId Identifier of order.
	 * @param SuccessCallback Callback function called after the payment was successfully completed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckPendingOrder(const FString& AccessToken, const int32 OrderId,
		const FOnStoreSuccessPayment& SuccessCallback, const FOnError& ErrorCallback);

	/** Create Order With Specified Free Item
	 * Creates an order with a specified free item.
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired free item SKU.
	 * @param Currency (optional) Desired payment currency. Leave empty to use the default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use the default value.
	 * @param CustomParameters (optional) Map of custom parameters. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after the payment was successfully completed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Quantity Item quantity.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateOrderWithSpecifiedFreeItem(const FString& AuthToken, const FString& ItemSKU,
		const FString& Currency, const FString& Locale,
		const FXsollaParameters CustomParameters,
		const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback, const int32 Quantity = 1);

	/** Create Order With Free Cart
	 * Creates order with free cart.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of the cart for the purchase. The current user cart will be purchased if empty.
	 * @param Currency (optional) Desired payment currency. Leave empty to use the default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use the default value.
	 * @param CustomParameters (optional) Map of custom parameters. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after the payment was successfully completed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateOrderWithFreeCart(const FString& AuthToken, const FString& CartId,
		const FString& Currency, const FString& Locale,
		const FXsollaParameters CustomParameters,
		const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Clear Cart
	 * Removes all items from the cart.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of a cart to be cleared. The current user cart will be cleared if empty.
	 * @param SuccessCallback Callback function called after successful cart clearing.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ClearCart(const FString& AuthToken, const FString& CartId,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Update Cart
	 * Updates cart content (cached locally).
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of the cart to be updated. The current user cart will be updated if empty.
	 * @param Currency The currency in which prices are displayed (USD by default). Three-letter currency code per ISO 4217.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after local cache of cart items was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateCart(const FString& AuthToken, const FString& CartId,
		const FString& Currency, const FString& Locale,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Add to Cart
	 * Adds an item to the cart and changes its quantity.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of a cart to which item will be added. The current user cart will be modified if empty.
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Amount of items to be added to the cart.
	 * @param SuccessCallback Callback function called after successfully adding a new item to the cart.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AddToCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, const int32 Quantity,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Remove from Cart
	 * Completely removes an item from the cart.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of a cart from which the item will be removed. The current user cart will be modified if empty.
	 * @param ItemSKU Desired item SKU.
	 * @param SuccessCallback Callback function called after successfully removing an item from the cart.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveFromCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Fill Cart By ID
	 * Fills out the specific cart with items. If the cart already has an item, the existing item position will be replaced by the given value.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart which will be filled. The current user cart will be filled if empty.
	 * @param Items Item for filling the cart.
	 * @param SuccessCallback Callback function called after cart is successfully filled.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FillCartById(const FString& AuthToken, const FString& CartId, const TArray<FStoreCartItem>& Items,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Specified Bundle
	* Gets a specified bundle.
	*
	* @param Sku Bundle SKU.
	* @param SuccessCallback Callback function called after the cart is successfully filled.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	* @param AuthToken User JWT obtained during authorization using Xsolla Login ([Bearer token](https://developers.xsolla.com/api/login/overview/#section/Authentication/Getting-a-user-token)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Bundle", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSpecifiedBundle(const FString& Sku, const FOnGetSpecifiedBundleUpdate& SuccessCallback, const FOnError& ErrorCallback, const FString& AuthToken = TEXT(""));

	/** Get Bundles
	* Gets a list of bundles for building a catalog.
	*
	* @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	* @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address if not specified.
	* @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	* @param SuccessCallback Callback function called after bundles are successfully received.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page.
	* @param Offset Number of the element from which the list is generated (the count starts from 0).
	* @param AuthToken User JWT obtained during authorization using Xsolla Login ([Bearer token](https://developers.xsolla.com/api/login/overview/#section/Authentication/Getting-a-user-token)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Bundle", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetBundles(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetListOfBundlesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Get Virtual Currency
	 * Gets virtual currency with specified SKU.
	 *
	 * @param CurrencySKU Desired currency SKU.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after successful request of specified virtual currency data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrency(const FString& CurrencySKU,
		const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnCurrencyUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Virtual Currency Package
	 * Gets virtual currency package with specified SKU.
	 *
	 * @param PackageSKU Desired currency package SKU
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields 'media_list', 'order' and 'long_description'.
	 * @param SuccessCallback Callback function called after successful request of specified virtual currency package data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyPackage(const FString& PackageSKU,
		const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnCurrencyPackageUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Buy Item with Virtual Currency
	 * Buys an item using virtual currency.
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param CurrencySKU Currency to buy virtual items with.
	 * @param Platform Target platform
	 * @param SuccessCallback Callback function called after the successful item purchase.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void BuyItemWithVirtualCurrency(const FString& AuthToken, const FString& ItemSKU, const FString& CurrencySKU,
		const EXsollaPublishingPlatform Platform, const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Promocode Rewards
	 * Gets promo code rewards by its code. Can be used to let users choose one of many items as a bonus.
	 * The usual case is choosing a DRM if the promo code contains a game as a bonus (type=unit).
	 *
	 * @param AuthToken User authorization token.
	 * @param PromocodeCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Callback function called after successfully receiving promocode rewards.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetPromocodeRewards(const FString& AuthToken, const FString& PromocodeCode,
		const FOnGetPromocodeRewardsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Redeem Promocode
	 * Redeems a promo code. After redeeming a promo code, the user will get free items and/or the price of cart will be decreased.
	 *
	 * @param AuthToken User authorization token.
	 * @param PromocodeCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Callback function called after successful promocode redemption.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemPromocode(const FString& AuthToken, const FString& PromocodeCode,
		const FOnPromocodeUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Remove Promocode From Cart
	 * Removes a promo code from a cart. After the promo code is removed, the total price of all items in the cart will be recalculated without bonuses and discounts provided by a promo code.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful promocode redemption.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemovePromocodeFromCart(const FString& AuthToken,
		const FOnPromocodeUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Games
	 * Gets list of games for building a catalog.
	 *
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calcualtions are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after successful request of specified games list data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys" , meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGamesList(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnStoreGamesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Games By Specified Group
	 * Gets the list of games from the specified group for building a catalog.
	 *
	 * @param ExternalId Group external ID.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculations are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after successful request of specified game list data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGamesListBySpecifiedGroup(const FString& ExternalId, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetGamesListBySpecifiedGroup& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Game Item
	 * Gets a game item with the specified SKU for the catalog.
	 *
	 * @param GameSKU Desired game SKU.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculations are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after successful request of specified game data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGameItem(const FString& GameSKU, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGameUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Game Key Item
	 * Gets a game key item with the specified SKU for the catalog.
	 *
	 * @param ItemSKU Desired game item SKU.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculations are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after successful request of specified game data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGameKeyItem(const FString& ItemSKU, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGameKeyUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Game Key List
	 * Gets a game key list from the specified group for building a catalog.
	 *
	 * @param ExternalId Group external ID.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per ISO 3166-1 alpha-2. Calculations are based on the user's IP address in not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Callback function called after successful request of specified game key data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGameKeysListBySpecifiedGroup(const FString& ExternalId, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetGameKeysListBySpecifiedGroup& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get DRM List
	 * Gets the list of available DRMs.
	 *
	 * @param SuccessCallback Callback function called after successful request of specified DRM data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetDRMList(const FOnDRMListUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Owned Games
	 * Gets the list of games owned by the user. The response will contain an array of games owned by a particular user.
	 *
	 * @param AuthToken User authorization token.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields 'attributes'.
	 * @param SuccessCallback Callback function called after successful request of specified owned games data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param bIsSandbox What type of entitlements should be returned. If the parameter is set to true, the entitlements received by the user in the sandbox mode only are returned.
	 * If the parameter is set to false, the entitlements received by the user in the live mode only are returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetOwnedGames(const FString& AuthToken, const TArray<FString>& AdditionalFields,
		const FOnOwnedGamesListUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const bool bIsSandbox = false);

	/** Redeem Game Code By Client
	* Grants an entitlement by a provided game code.
	*
	* @param AuthToken User authorization token.
	* @param Code Game code.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemGameCodeByClient(const FString& AuthToken, const FString& Code,
		const FOnRedeemGameCodeSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Subscription Public Plans
	* Returns a list of all plans, including plans purchased by the user while promotions are active.
	*
	* @param PlanId Array of subscription plan IDs. Plan ID can be found in the URL of the subscription details page in Publisher Account (https://publisher.xsolla.com/{merchant_id}/projects/{project_id}/subscriptions/plans/{merplan_id}).
	* @param PlanExternalId Array of subscription plan external IDs (32 characters per ID). Plan external ID can be found in Publisher Account in the **Subscriptions > Subscription plans** section next to the plan name.
	* @param Country User's country. Affects the choice of locale and currency. By default, it is determined by the user's IP address.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address. Can be enforced by using an ISO 639-1 code.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page (15 elements are displayed by default).
	* @param Offset Number of elements from which the list is generated (the count starts from 0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "PlanId, PlanExternalId, SuccessCallback, ErrorCallback"))
	void GetSubscriptionPublicPlans(const TArray<int> PlanId, const TArray<FString>& PlanExternalId, const FString& Country, const FString& Locale,
		const FOnSubscriptionPublicPlansListUpdate& SuccessCallback, const FOnError& ErrorCallback,	const int Limit = 50, const int Offset = 0);

	/** Get Subscription Plans
	* Returns a list of all plans, including plans purchased by the user while promotions are active.
	*
	* @param AuthToken User authorization token.
	* @param PlanId Array of subscription plan IDs. Plan ID can be found in the URL of the subscription details page in Publisher Account (https://publisher.xsolla.com/{merchant_id}/projects/{project_id}/subscriptions/plans/{merplan_id}).
	* @param PlanExternalId Array of subscription plan external IDs (32 characters per ID). Plan external ID can be found in Publisher Account in the **Subscriptions > Subscription plans** section next to the plan name.
	* @param Country User's country. Affects the choice of locale and currency. By default, it is determined by the user's IP address.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address. Can be enforced by using an ISO 639-1 code.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page (15 elements are displayed by default).
	* @param Offset Number of elements from which the list is generated (the count starts from 0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "PlanId, PlanExternalId, SuccessCallback, ErrorCallback"))
	void GetSubscriptionPlans(const FString& AuthToken, const TArray<int> PlanId, const TArray<FString>& PlanExternalId, const FString& Country, const FString& Locale,
		const FOnSubscriptionPlansListUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Subscriptions
	* Returns a list of active recurrent subscriptions that have the status `active`, `non renewing`, and `pause`.
	*
	* @param AuthToken User authorization token.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address. Can be enforced by using an ISO 639-1 code.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page (15 elements are displayed by default).
	* @param Offset Number of elements from which the list is generated (the count starts from 0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptions(const FString& AuthToken, const FString& Locale,
		const FOnSubscriptionsListUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Subscriptions Details
	* Returns information about a subscription by its ID. Subscription can be have any status.
	*
	* @param AuthToken User authorization token.
	* @param SubscriptionId Subscription ID. **Required**.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address. Can be enforced by using an ISO 639-1 code.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionDetails(const FString& AuthToken, const int32 SubscriptionId, const FString& Locale,
		const FOnGetSubscriptionDetailsSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Subscription Purchase Url
	* Returns Pay Station URL for the subscription purchase.
	*
	* @param AuthToken User authorization token.
	* @param PlanExternalId Subscription plan external ID (32 characters). Plan external ID can be found in Publisher Account in the **Subscriptions > Subscription plans** section next to the plan name.
	* @param Country User's country. Affects the choice of locale and currency. By default, it is determined by the user's IP address.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionPurchaseUrl(const FString& AuthToken, const FString& PlanExternalId, const FString& Country,
		const FOnGetSubscriptionPayStationLinkSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Subscription Management Url
	* Returns Pay Station URL for the subscription management.
	*
	* @param AuthToken User authorization token.
	* @param Country User's country. Affects the choice of locale and currency. By default, it is determined by the user's IP address.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionManagementUrl(const FString& AuthToken, const FString& Country,
		const FOnGetSubscriptionPayStationLinkSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Subscription Renewal Url
	* Returns Pay Station URL for the subscription renewal.
	*
	* @param AuthToken User authorization token.
	* @param SubscriptionId Subscription ID. **Required**.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionRenewalUrl(const FString& AuthToken, const int32 SubscriptionId,
		const FOnGetSubscriptionPayStationLinkSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Cancel Subscription
	* Changes a regular subscription status to non_renewing (subscription is automatically canceled after expiration).
	*
	* @param AuthToken User authorization token.
	* @param SubscriptionId Subscription ID. **Required**.
	* @param SuccessCallback Callback function called after successful redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CancelSubscription(const FString& AuthToken, const int32 SubscriptionId,
		const FOnCancelSubscriptionSuccess& SuccessCallback, const FOnError& ErrorCallback);

protected:
	void GetVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreItemsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetItemGroups_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnItemGroupsUpdate SuccessCallback, FOnError ErrorCallback);
	void GetVirtualCurrencies_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnVirtualCurrenciesUpdate SuccessCallback, FOnError ErrorCallback);
	void GetVirtualCurrencyPackages_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnVirtualCurrencyPackagesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetItemsListBySpecifiedGroup_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetItemsListBySpecifiedGroup SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetAllItemsList_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetItemsList SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void CreateOrderWithSpecifiedFreeItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void CreateOrderWithFreeCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void CreateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnError ErrorCallback);
	void ClearCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnError ErrorCallback);
	void UpdateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnError ErrorCallback);
	void AddToCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnError ErrorCallback);
	void RemoveFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnError ErrorCallback);
	void FillCartById_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetListOfBundles_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetListOfBundlesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetSpecifiedBundle_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetSpecifiedBundleUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCurrencyUpdate SuccessCallback, FOnError ErrorCallback);
	void GetVirtualCurrencyPackage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCurrencyPackageUpdate SuccessCallback, FOnError ErrorCallback);

	void BuyItemWithVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetPromocodeRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetPromocodeRewardsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void RedeemPromocode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnPromocodeUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void RemovePromocodeFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnPromocodeUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetGamesList_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnStoreGamesUpdate SuccessCallback, FOnError ErrorCallback);

	void GetGamesListBySpecifiedGroup_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetGamesListBySpecifiedGroup SuccessCallback, FOnError ErrorCallback);

	void GetGameItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGameUpdate SuccessCallback, FOnError ErrorCallback);

	void GetGameKeyItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGameKeyUpdate SuccessCallback, FOnError ErrorCallback);

	void GetGameKeysListBySpecifiedGroup_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetGameKeysListBySpecifiedGroup SuccessCallback, FOnError ErrorCallback);

	void GetDRMList_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnDRMListUpdate SuccessCallback, FOnError ErrorCallback);

	void GetOwnedGames_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnOwnedGamesListUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void RedeemGameCodeByClient_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnRedeemGameCodeSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetSubscriptionPublicPlans_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnSubscriptionPublicPlansListUpdate SuccessCallback, FOnError ErrorCallback);

	void GetSubscriptionPlans_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnSubscriptionPlansListUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetSubscriptions_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnSubscriptionsListUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetSubscriptionDetails_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetSubscriptionDetailsSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void GetSubscriptionPaystationLink_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnGetSubscriptionPayStationLinkSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	void CancelSubscription_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCancelSubscriptionSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	/** Return true if error is happened */
	void HandleRequestError(XsollaHttpRequestError ErrorData, FOnError ErrorCallback);

	void HandlePurchaseFreeItemsRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

protected:
	/** Load save game and extract data */
	void LoadData();

	/** Save cached data or reset one if necessary */
	void SaveData();

	/** Check whether sandbox is enabled */
	bool IsSandboxEnabled() const;

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET,
		const FString& AuthToken = FString(), const FString& Content = FString());

	/** Serialize json object into string */
	FString SerializeJson(const TSharedPtr<FJsonObject> DataJson) const;

	/** Try to execute next request in queue */
	void ProcessNextCartRequest();

	/** Prepare payload for payment token request */
	TSharedPtr<FJsonObject> PreparePaymentTokenRequestPayload(const FString& Currency, const FString& Country, const FString& Locale, const FXsollaParameters& CustomParameters);

	/** Prepare paystation settings */
	TSharedPtr<FJsonObject> PreparePaystationSettings();

	/** Get payment interface theme */
	FString GetPaymentInerfaceTheme() const;

	/** Extract Steam user ID from auth token */
	bool GetSteamUserId(const FString& AuthToken, FString& SteamId, FString& OutError);

	/** Queue to store cart change requests */
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> CartRequestsQueue;

public:
	/** Gets the list of cached virtual items without any Category provided. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	TArray<FStoreItem> GetVirtualItemsWithoutGroup() const;

	/** Gets cached items data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FStoreItemsData& GetItemsData() const;

	/** Gets cached cart data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart")
	const FStoreCart& GetCart() const;

	/** Gets the pending PayStation URL to be opened in browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FString& GetPendingPaystationUrl() const;

	/** Gets name of the cached item with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FString GetItemName(const FString& ItemSKU) const;

	/** Gets item from the cache with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FStoreItem& FindItemBySku(const FString& ItemSku, bool& bHasFound) const;

	/** Gets package from the cache with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FVirtualCurrencyPackage& FindVirtualCurrencyPackageBySku(const FString& ItemSku, bool& bHasFound) const;

	/** Checks if the certain item is in the cart. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart")
	bool IsItemInCart(const FString& ItemSKU) const;

public:
	/** Event occurred when the cart was changed or updated. */
	UPROPERTY(BlueprintAssignable, Category = "Xsolla|Store|Cart")
	FOnCartUpdate OnCartUpdate;

protected:
	/** Cached Xsolla Store project id */
	FString ProjectID;

	/** Cached virtual items list */
	FStoreItemsData ItemsData;

	/** Current cart */
	FStoreCart Cart;

	/** Cached virtual currency packages */
	FVirtualCurrencyPackagesData VirtualCurrencyPackages;

	/** Cached cart desired currency (used for silent cart update) */
	FString CachedCartCurrency;

	/** Cached auth token (used for silent cart update) */
	FString CachedAuthToken;

	/** Cached cart identifier (used for silent cart update) */
	FString CachedCartId;

	/** Cached cart locale (used for silent cart update) */
	FString CachedCartLocale;

	/** Pending PayStation URL to be opened in browser */
	FString PengindPaystationUrl;

	UXsollaStoreBrowserWrapper* MyBrowser;

private:
	UPROPERTY()
	TSubclassOf<UXsollaStoreBrowserWrapper> DefaultBrowserWidgetClass;

	UPROPERTY()
	TArray<UXsollaOrderCheckObject*> CachedOrderCheckObjects;

	UPROPERTY()
	UXsollaLoginSubsystem* LoginSubsystem;
};
