// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

#include "XsollaStoreSubsystem.generated.h"

/** Verb (GET, PUT, POST) used by the request. */
UENUM(BlueprintType)
enum class EXsollaRequestVerb : uint8
{
	GET,
	POST,
	PUT,
	DELETE
};

class FJsonObject;

DECLARE_DYNAMIC_DELEGATE(FOnStoreUpdate);
DECLARE_DYNAMIC_DELEGATE(FOnStoreCartUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCartUpdate, const FStoreCart&, Cart);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnStoreError, int32, StatusCode, int32, ErrorCode, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchTokenSuccess, const FString&, AccessToken, int32, OrderId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCheckOrder, int32, OrderId, EXsollaOrderStatus, OrderStatus);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCurrencyUpdate, const FVirtualCurrency&, Currency);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCurrencyPackageUpdate, const FVirtualCurrencyPackage&, CurrencyPackage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPurchaseUpdate, int32, OrderId);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetPromocodeRewardsUpdate, FStorePromocodeRewardData, RewardsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetListOfBundlesUpdate, FStoreListOfBundles, ListOfBundles);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSpecifiedBundleUpdate, FStoreBundle, Bundle);
DECLARE_DYNAMIC_DELEGATE(FOnRedeemPromocodeUpdate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetItemsListBySpecifiedGroup, FStoreItemsList, ItemsList);

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
	 * @param InProjectId New Project ID value form Publisher Account > Project settings > Project ID.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void Initialize(const FString& InProjectId);

	/** Update Virtual Items
	 * Updates the list of virtual items available for the configured project (cached locally).
	 *
	 * @param SuccessCallback Callback function called after local cache of virtual items was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param Limit Limit for the number of elements on the page.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Update Item Groups
	 * Updates the list of virtual item groups (cached locally).
	 *
	 * @param Locale (optional) Response language (e.g. item name, item description). Two-letter lowercase language code per ISO 639-1. Leave empty to use the default value.
	 * @param SuccessCallback Callback function called after local cache of virtual item groups was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param Limit Limit for the number of elements on the page.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateItemGroups(const FString& Locale, const FOnStoreUpdate& SuccessCallback,
		const FOnStoreError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Update Virtual Currencies
	 * Updates the list of virtual currencies (cached locally).
	 *
	 * @param SuccessCallback Callback function called after local cache of virtual currencies was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param Limit Limit for the number of elements on the page.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualCurrencies(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Update Virtual Currency Packages
	 * Updates the list of virtual currency packages (cached locally).
	 *
	 * @param SuccessCallback Callback function called after local cache of virtual currency packages was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param Limit Limit for the number of elements on the page.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualCurrencyPackages(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Get Items List By Specified Group
	 * Gets an items list from the specified group for building a catalog.
	 *
	 * @param ExternalId Group external ID.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count start from 0).
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param AdditionalFields The list of additional fields. This fields will be in a response if you send it in a request. Available fields 'media_list', 'purchase_limit', 'promotion', 'order', and 'long_description'.
	 * @param SuccessCallback Callback function called after server response.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualItems", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetItemsListBySpecifiedGroup(const FString& ExternalId, const int Limit, const int Offset,
		const FString& Locale, const TArray<FString>& AdditionalFields,
		const FOnGetItemsListBySpecifiedGroup& SuccessCallback, const FOnStoreError& ErrorCallback);

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
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU,
		const FString& Currency, const FString& Country, const FString& Locale,
		const FXsollaPaymentCustomParameters CustomParameters,
		const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback);

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
		const FXsollaPaymentCustomParameters CustomParameters,
		const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Launch Payment Console
	 * Opens payment console for the provided access token.
	 *
	 * @param AccessToken Payment token used during purchase processing.
	 * @param BrowserWidget Widget to represent a payment form. Can be set in the project settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void LaunchPaymentConsole(const FString& AccessToken, UUserWidget*& BrowserWidget);

	/** Check Order
	 * Checks pending order status by its ID.
	 *
	 * @param AuthToken User authorization token.
	 * @param OrderId Identifier of order to be checked.
	 * @param SuccessCallback Callback function called after successful order check. Order status will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckOrder(const FString& AuthToken, int32 OrderId,
		const FOnCheckOrder& SuccessCallback, const FOnStoreError& ErrorCallback);

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
		const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update Cart
	 * Updates cart content (cached locally).
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart to be updated. The current user cart will be updated if empty.
	 * @param SuccessCallback Callback function called after local cache of cart items was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateCart(const FString& AuthToken, const FString& CartId,
		const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Add to Cart
	 * Adds an item to the cart and changes its quantity.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of a cart to which item will be added. The current user cart will be modified if empty.
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Amount of items to be added to a cart.
	 * @param SuccessCallback Callback function called after adding a new item to the cart successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AddToCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, int32 Quantity,
		const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

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
		const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Fill Cart By ID
	 * Fills the specific cart with items. If the cart already has an item, the existing item position will be replaced by the given value.
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart which will fill. The current user cart will be filled if empty.
	 * @param Items Item for fill cart.
	 * @param SuccessCallback Callback function called after cart filled successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FillCartById(const FString& AuthToken, const FString& CartId, const TArray<FStoreCartItem>& Items,
		const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Get Specified Bundle
	* Gets a specified bundle.
	*
	* @param Sku Bundle SKU.
	* @param SuccessCallback Callback function called after the cart is filled successfully.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Bundle", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSpecifiedBundle(const FString& Sku,
		const FOnGetSpecifiedBundleUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update Bundles
	* Gets a list of bundles for building a catalog.
	*
	* @param Locale Response language. Tow-letter lowercase language code per ISO 639-1.
	* @param SuccessCallback Callback function called after cart is successfully filled .
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Bundle", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateBundles(const FString& Locale,
		const FOnGetListOfBundlesUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Get Virtual Currency
	 * Gets virtual currency with specified SKU.
	 *
	 * @param CurrencySKU Desired currency SKU
	 * @param SuccessCallback Callback function called after successful request of specified virtual currency data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetVirtualCurrency(const FString& CurrencySKU,
		const FOnCurrencyUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Get Virtual Currency Package
	 * Gets virtual currency package with specified SKU.
	 *
	 * @param PackageSKU Desired currency package SKU
	 * @param SuccessCallback Callback function called after successful request of specified virtual currency package data.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyPackage(const FString& PackageSKU,
		const FOnCurrencyPackageUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Buy Item with Virtual Currency
	 * Buys an item using virtual currency.
	 * 
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param CurrencySKU Currency to buy virtual items with.
	 * @param SuccessCallback Callback function called after the successful item purchase.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void BuyItemWithVirtualCurrency(const FString& AuthToken, const FString& ItemSKU, const FString& CurrencySKU,
		const FOnPurchaseUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Get Promocode Rewards
	* Gets promo code rewards by its code. Can be used to allow users to choose one of many items as a bonus.
	* The usual case is choosing a DRM if the promo code contains a game as a bonus (type=unit).
	* 
	* @param AuthToken User authorization token.
	* @param PromocodeCode Unique case sensitive code. Contains letters and numbers.
	* @param SuccessCallback Callback function called after receiving promocode rewards successfully.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetPromocodeRewards(const FString& AuthToken, const FString& PromocodeCode,
		const FOnGetPromocodeRewardsUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Redeem Promocode
	* Redeems a promo code. After redeeming a promo code, the user will get free items and/or the price of cart will be decreased.
	* 
	* @param AuthToken User authorization token.
	* @param PromocodeCode Uniques case sensitive code. Contains letters and numbers.
	* @param SuccessCallback Callback function called after successful promocode redemption.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemPromocode(const FString& AuthToken, const FString& PromocodeCode,
		const FOnRedeemPromocodeUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

protected:
	void UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateItemGroups_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateVirtualCurrencies_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateVirtualCurrencyPackages_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void GetItemsListBySpecifiedGroup_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnGetItemsListBySpecifiedGroup SuccessCallback, FOnStoreError ErrorCallback);

	void FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback);
	void CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnCheckOrder SuccessCallback, FOnStoreError ErrorCallback);

	void CreateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void ClearCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void AddToCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void RemoveFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void FillCartById_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void GetListOfBundles_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnGetListOfBundlesUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void GetSpecifiedBundle_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnGetSpecifiedBundleUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void GetVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnCurrencyUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void GetVirtualCurrencyPackage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnCurrencyPackageUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void BuyItemWithVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void GetPromocodeRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnGetPromocodeRewardsUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void RedeemPromocode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnRedeemPromocodeUpdate SuccessCallback, FOnStoreError ErrorCallback);

	/** Return true if error is happened */
	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		bool bSucceeded, FOnStoreError ErrorCallback);

protected:
	/** Load save game and extract data */
	void LoadData();

	/** Save cached data or reset one if necessary */
	void SaveData();

	/** Check whether sandbox is enabled */
	bool IsSandboxEnabled() const;

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url, const EXsollaRequestVerb Verb = EXsollaRequestVerb::GET,
		const FString& AuthToken = FString(), const FString& Content = FString());

	/** Serialize json object into string */
	FString SerializeJson(const TSharedPtr<FJsonObject> DataJson) const;

	/** Parse JWT token and get its payload as json object */
	bool ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject) const;

	/** Try to execute next request in queue */
	void ProcessNextCartRequest();

	/** Get name of publishing platform */
	FString GetPublishingPlatformName();

	/** Queue to store cart change requests */
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> CartRequestsQueue;

public:
	/** Get Virtual Items
	 * Gets the list of cached virtual items filtered by category.
	 *
	 * @param GroupFilter Group for which items should be received.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	TArray<FStoreItem> GetVirtualItems(const FString& GroupFilter) const;

	/** Gets the list of cached virtual items without any Category provided. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	TArray<FStoreItem> GetVirtualItemsWithoutGroup() const;

	/** Gets cached items data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FStoreItemsData GetItemsData() const;

	/** Gets the list of cached virtual currencies. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency")
	TArray<FVirtualCurrency> GetVirtualCurrencyData() const;

	/** Gets the list of cached virtual currency packages. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency")
	TArray<FVirtualCurrencyPackage> GetVirtualCurrencyPackages() const;

	/** Gets cached cart data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart")
	FStoreCart GetCart() const;
	
	/** Gets the pending PayStation URL to be opened in browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FString GetPendingPaystationUrl() const;

	/** Gets name of the cached item with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FString GetItemName(const FString& ItemSKU) const;

	/** Gets name of the cached virtual currency with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FString GetVirtualCurrencyName(const FString& CurrencySKU) const;

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

	/** Cached list of virtual currencies */
	FVirtualCurrencyData VirtualCurrencyData;

	/** Cached virtual currency packages */
	FVirtualCurrencyPackagesData VirtualCurrencyPackages;

	/** Cached cart desired currency (used for silent cart update) */
	FString CachedCartCurrency;

	/** Cached auth token (used for silent cart update) */
	FString CachedAuthToken;

	/** Cached cart identifier (used for silent cart update) */
	FString CachedCartId;

	/** Pending paystation url to be opened in browser */
	FString PengindPaystationUrl;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;

	/** Patching JsonObject with CustomParameters */
	static void AddCustomParameters(TSharedPtr<FJsonObject> JsonObject, FXsollaPaymentCustomParameters CustomParameters);
};
