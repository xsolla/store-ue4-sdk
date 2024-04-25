// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaStoreDataModel.h"
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
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreBrowserClosed, bool, bIsManually);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCartUpdate, const FStoreCart&, Cart);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchTokenSuccess, const FString&, AccessToken, int32, OrderId);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnCheckOrder, int32, OrderId, EXsollaOrderStatus, OrderStatus, FXsollaOrderContent, OrderContent);
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
	 * @param InProjectId New Project ID value from Publisher Account. It can be found in Publisher Account next to the name of your project.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void Initialize(const FString& InProjectId);

	/** Returns a list of virtual items according to pagination settings. The list includes items for which display in the store is enabled in the settings. For each virtual item, complete data is returned.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order` and `long_description`.
	 * @param SuccessCallback Called after virtual items were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page. The maximum number of elements on a page is 50.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualItems(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnStoreItemsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Returns a full list of virtual item groups. The list includes groups for which display in the store is enabled in the settings
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param Locale (optional) Response language (e.g. item name, item description).<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param SuccessCallback Called after virtual item groups were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetItemGroups(const FString& Locale,
		const FOnItemGroupsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns a list of virtual currencies according to pagination settings.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). By default, it is determined by the user's IP address.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order` and `long_description`.
	 * @param SuccessCallback Called after virtual currencies were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page. The maximum number of elements on a page is 50.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencies(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnVirtualCurrenciesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns a list of virtual currency packages according to pagination settings. The list includes packages for which display in the store is enabled in the settings.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). By default, it is determined by the user's IP address.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after virtual currency packages were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page. The maximum number of elements on a page is 50.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyPackages(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnVirtualCurrencyPackagesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Returns a list of items for the specified group according to pagination settings. The list includes items for which display in the store is enabled in the settings. In the settings of the group, the display in the store must be enabled.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param ExternalId Group external ID.
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). By default, it is determined by the user's IP address.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after server response.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page. The maximum number of elements on a page is 50.
	 * @param Offset Number of the element from which the list is generated (the count start from 0).
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualItems", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetItemsListBySpecifiedGroup(const FString& ExternalId, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetItemsListBySpecifiedGroup& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Returns a full list of virtual items. The list includes items for which display in the store is enabled in the settings. For each virtual item, the SKU, name, description, and data about the groups it belongs to are returned.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param SuccessCallback Called after server response.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualItems", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetAllItemsList(const FString& Locale, const FOnGetItemsList& SuccessCallback, const FOnError& ErrorCallback, const FString& AuthToken = TEXT(""));

	/** Initiates an item purchase session and fetches token for payment console.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/one-click-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param ItemSKU Desired item SKU.
	 * @param SuccessCallback Called after payment token was successfully fetched.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param PurchaseParams (optional) Purchase and payment UI parameters, such as `locale`, `currency`, etc.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback, PurchaseParams"))
	void FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU,
		const FOnFetchTokenSuccess& SuccessCallback, const FOnError& ErrorCallback, const FXsollaPaymentTokenRequestPayload& PurchaseParams);

	/** Creates an order with items from the cart with the specified ID or from the cart of the current user. Returns the payment token and order ID.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of the cart for the purchase. The current user cart will be purchased if empty.
	 * @param SuccessCallback Called after the payment token was successfully fetched.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param PurchaseParams (optional) Purchase and payment UI parameters, such as `locale`, `currency`, etc.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback, PurchaseParams"))
	void FetchCartPaymentToken(const FString& AuthToken, const FString& CartId,
		const FOnFetchTokenSuccess& SuccessCallback, const FOnError& ErrorCallback, const FXsollaPaymentTokenRequestPayload& PurchaseParams);

	/** Opens payment UI for the provided access token.
	 * More about the use cases:
	 * - [Cart purchase](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/)
	 * - [Purchase in one click](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/one-click-purchase/)
	 * - [Ordering free items](https://developers.xsolla.com/sdk/unreal-engine/promo/free-items/)
	 *
	 * @param WorldContextObject The world context.
	 * @param OrderId Order identifier.
	 * @param AccessToken Payment token used during purchase processing.
	 * @param SuccessCallback Called after the payment was successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param BrowserClosedCallback Called after the browser is closed. The event is tracked only when the payment UI is opened in the built-in browser. External browser events can't be tracked.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "SuccessCallback, ErrorCallback, BrowserClosedCallback"))
	void LaunchPaymentConsole(UObject* WorldContextObject, const int32 OrderId, const FString& AccessToken,
		const FOnStoreSuccessPayment& SuccessCallback, const FOnError& ErrorCallback, const FOnStoreBrowserClosed& BrowserClosedCallback);

	/** Checks pending order status by its ID.
	 *
	 * @param AuthToken User authorization token.
	 * @param OrderId Identifier of order to be checked.
	 * @param SuccessCallback Callback function called after successful order check. Order status will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckOrder(const FString& AuthToken, const int32 OrderId,
		const FOnCheckOrder& SuccessCallback, const FOnError& ErrorCallback);

	/** Checks pending order.
	 *
	 * @param AccessToken Payment token used during purchase processing.
	 * @param OrderId Identifier of order.
	 * @param SuccessCallback Called after the payment was successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param bIsUserInvolvedToPayment Whether payment UI involved in purchase. `true` for purchase for real currency. `false` for free item purchase and purchase for virtual currency
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckPendingOrder(const FString& AccessToken, const int32 OrderId,
		const FOnStoreSuccessPayment& SuccessCallback, const FOnError& ErrorCallback, bool bIsUserInvolvedToPayment = false);

	/** Create order with specified free item. The created order will get a `done` order status.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/free-items/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param ItemSKU Desired free item SKU.
	 * @param SuccessCallback Called after the payment was successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Quantity Item quantity.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateOrderWithSpecifiedFreeItem(const FString& AuthToken, const FString& ItemSKU,
		const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback, const int32 Quantity = 1);

	/** Create order with free cart. The created order will get a `done` order status.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/free-items/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of the cart for the purchase. The current user cart will be purchased if empty.
	 * @param SuccessCallback Called after the payment was successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateOrderWithFreeCart(const FString& AuthToken, const FString& CartId,
		const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Initiates purchase by passing store item
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param StoreItem Desired store item.
	 * @param PurchaseParams (optional) Purchase and payment UI parameters, such as `locale`, `currency`, etc.
	 * @param SuccessCallback Called after the payment was successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "PurchaseParams, SuccessCallback, ErrorCallback"))
	void PurchaseStoreItem(const FString& AuthToken, const FStoreItem& StoreItem, const FXsollaPaymentTokenRequestPayload& PurchaseParams,
		const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Initiate purchase by passing virtual currency package
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CurrencyPackage Desired currency package.
	 * @param PurchaseParams (optional) Purchase and payment UI parameters, such as `locale`, `currency`, etc.
	 * @param SuccessCallback Called after the payment was successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "PurchaseParams, SuccessCallback, ErrorCallback"))
	void PurchaseCurrencyPackage(const FString& AuthToken, const FVirtualCurrencyPackage& CurrencyPackage, const FXsollaPaymentTokenRequestPayload& PurchaseParams,
		const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Removes all items from the cart with the specified ID or from the cart of the current user.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of a cart to be cleared.
	 * @param SuccessCallback Called after successful cart clearing.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ClearCart(const FString& AuthToken, const FString& CartId,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of items from the cart with the specified ID or from the cart of the current user. For each item, complete data is returned.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of the cart to be updated. The current user cart will be updated if empty.
	 * @param Currency The currency in which prices are displayed (USD by default). Three-letter currency code per [ISO 4217](https://en.wikipedia.org/wiki/ISO_4217).
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).
	 * @param SuccessCallback Called after local cache of cart items was successfully updated.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateCart(const FString& AuthToken, const FString& CartId,
		const FString& Currency, const FString& Locale,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Updates the quantity of a previously added item in the cart with the specified ID or in the current user cart. If there is no item with the specified SKU in the cart, it will be added.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of a cart to which item will be added. The current user cart will be modified if empty.
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Number of items to be added to the cart.
	 * @param SuccessCallback Called after successfully adding a new item to the cart.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AddToCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, const int32 Quantity,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Removes the item from the cart with the specified ID or from the cart of the current user.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of a cart from which the item will be removed.
	 * @param ItemSKU Desired item SKU.
	 * @param SuccessCallback Called after successfully removing an item from the cart.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveFromCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Fills the cart with the specified ID or the cart of the current user with items. If there is already an item with the same SKU in the cart, the existing item position will be replaced by the passed value.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/cart-purchase/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param CartId (optional) Identifier of cart which will be filled.
	 * @param Items Item for filling the cart. If there is already an item with the same SKU in the cart, the existing item position will be replaced by the passed value.
	 * @param SuccessCallback Called after cart is successfully filled.
	 * @param ErrorCallback Called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FillCartById(const FString& AuthToken, const FString& CartId, const TArray<FStoreCartItem>& Items,
		const FOnStoreCartUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns information about the contents of the specified bundle. In the bundle settings, display in the store must be enabled.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/#unreal_engine_sdk_how_to_bundles).
	*
	* @param Sku Bundle SKU.
	* @param SuccessCallback Called after the cart is successfully filled.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Bundle", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSpecifiedBundle(const FString& Sku, const FOnGetSpecifiedBundleUpdate& SuccessCallback, const FOnError& ErrorCallback, const FString& AuthToken = TEXT(""));

	/** Returns a list of bundles according to pagination settings. The list includes bundles for which display in the store is enabled in the settings.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/#unreal_engine_sdk_how_to_bundles).
	*
	* @param Locale Response language.<br>
	* The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).
	* @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculated based on the user's IP address if not specified.
	* @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	* @param SuccessCallback Called after bundles are successfully received.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page. The maximum number of elements on a page is 50.
	* @param Offset Number of the element from which the list is generated (the count starts from 0).
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)). Can be empty. If specified, the method returns items that match the personalization rules for the current user.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Bundle", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetBundles(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetListOfBundlesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const FString& AuthToken = TEXT(""));

	/** Returns virtual currency with specified SKU.
	 *
	 * @param CurrencySKU Desired currency SKU.
	 * @param Locale Response language.<br>
	 * The following languages are supported: The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after successful request of specified virtual currency data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrency(const FString& CurrencySKU,
		const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnCurrencyUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of virtual currency packages according to pagination settings. The maximum number of elements on a page is 50. The list includes packages for which display in the store is enabled in the settings.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/catalog/catalog-display/).
	 *
	 * @param PackageSKU Desired currency package SKU.
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).<br>
	 * Leave empty to use the default value.
	 * @param Country Country to calculate regional prices and restrictions to catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculated based on the user's IP address if not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order` and `long_description`.
	 * @param SuccessCallback Called after successful request of specified virtual currency package data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyPackage(const FString& PackageSKU,
		const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnCurrencyPackageUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Creates an order with a specified item. The created order will get a `new` order status.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/item-purchase/purchase-for-vc/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param ItemSKU Desired item SKU.
	 * @param CurrencySKU Currency to buy virtual items with. Three-letter currency code per [ISO 4217](https://en.wikipedia.org/wiki/ISO_4217) (USD by default).
	 * @param Platform Publishing platform the user plays on.<br>
	 * Can be `xsolla` (default), `playstation_network`, `xbox_live`, `pc_standalone`, `nintendo_shop`, `google_play`, `app_store_ios`, `android_standalone`, `ios_standalone`, `android_other`, `ios_other`, or `pc_other`.
	 * @param SuccessCallback Called after the successful item purchase.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void BuyItemWithVirtualCurrency(const FString& AuthToken, const FString& ItemSKU, const FString& CurrencySKU,
		const EXsollaPublishingPlatform Platform, const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of items that can be credited to the user when the promo code is redeemed. Allows users to choose from several available items.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/promo-codes/#sdk_promo_codes).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param PromocodeCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Called after successfully receiving promocode rewards.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetPromocodeRewards(const FString& AuthToken, const FString& PromocodeCode,
		const FOnGetPromocodeRewardsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Redeems a promo code. After activating the promo code, the user gets free items and/or the price of the cart is reduced.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/promo-codes/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param PromocodeCode Unique case sensitive code. Contains letters and numbers.
	 * @param SuccessCallback Called after successful promocode redemption.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemPromocode(const FString& AuthToken, const FString& PromocodeCode,
		const FOnPromocodeUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Removes a promo code from a cart. After the promo code is removed, the total price of all items in the cart will be recalculated without bonuses and discounts provided by a promo code
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/promo/promo-codes/).
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param SuccessCallback Called after successful promocode redemption.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Promocode", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemovePromocodeFromCart(const FString& AuthToken,
		const FOnPromocodeUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns list of games for building a catalog.
	 *
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calcualtions are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after successful request of specified games list data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys" , meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGamesList(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnStoreGamesUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns the list of games from the specified group for building a catalog.
	 *
	 * @param ExternalId Group external ID.
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculations are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after successful request of specified game list data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGamesListBySpecifiedGroup(const FString& ExternalId, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetGamesListBySpecifiedGroup& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns a game item with the specified SKU for the catalog.
	 *
	 * @param GameSKU Desired game SKU.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculations are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after successful request of specified game data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGameItem(const FString& GameSKU, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGameUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Game Key Item
	 * Returns a game key item with the specified SKU for the catalog.
	 *
	 * @param ItemSKU Desired game item SKU.
	 * @param Locale Response language. Two-letter lowercase language code per ISO 639-1.
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculations are based on the user's IP address if the country is not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after successful request of specified game data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGameKeyItem(const FString& ItemSKU, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGameKeyUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a game key list from the specified group for building a catalog.
	 *
	 * @param ExternalId Group external ID.
	 * @param Locale Response language.<br>
	 * The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`, default).
	 * @param Country Country for which to calculate regional prices and restrictions in a catalog. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Calculations are based on the user's IP address in not specified.
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `media_list`, `order`, and `long_description`.
	 * @param SuccessCallback Called after successful request of specified game key data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetGameKeysListBySpecifiedGroup(const FString& ExternalId, const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
		const FOnGetGameKeysListBySpecifiedGroup& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns the list of available DRMs.
	 *
	 * @param SuccessCallback Called after successful request of specified DRM data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetDRMList(const FOnDRMListUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns the list of games owned by the user. The response will contain an array of games owned by a particular user.
	 *
	 * @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	 * @param AdditionalFields The list of additional fields. These fields will be in a response if you send it in a request. Available fields `attributes`.
	 * @param SuccessCallback Called after successful request of specified owned games data.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Limit Limit for the number of elements on the page.
	 * @param Offset Number of the element from which the list is generated (the count starts from 0).
	 * @param bIsSandbox What type of entitlements should be returned. If the parameter is set to true, the entitlements received by the user in the sandbox mode only are returned.
	 * If the parameter is set to `false`, the entitlements received by the user in the live mode only are returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetOwnedGames(const FString& AuthToken, const TArray<FString>& AdditionalFields,
		const FOnOwnedGamesListUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0, const bool bIsSandbox = false);

	/** Grants an entitlement by a provided game code.
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param Code Game code.
	* @param SuccessCallback Called after successful redemption.
	* @param ErrorCallback Called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|GameKeys", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RedeemGameCodeByClient(const FString& AuthToken, const FString& Code,
		const FOnRedeemGameCodeSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of all plans, including plans purchased by the user while promotions are active.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscriptions-purchase/).
	*
	* @param PlanId Array of subscription plan IDs. Plan ID can be found in the URL of the subscription details page in Publisher Account (https://publisher.xsolla.com/{merchant_id}/projects/{project_id}/subscriptions/plans/{merplan_id}).
	* @param PlanExternalId Array of subscription plan external IDs (32 characters per ID). Plan external ID can be found in Publisher Account in the **Subscriptions > Subscription plans** section next to the plan name.
	* @param User's country. Affects the choice of locale and currency. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). By default, it is determined by the user's IP address.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address.<br>
	* Can be enforced by using language code: The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`).
	* @param SuccessCallback Called after public plans have been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page (15 elements are displayed by default).
	* @param Offset Number of elements from which the list is generated (the count starts from 0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "PlanId, PlanExternalId, SuccessCallback, ErrorCallback"))
	void GetSubscriptionPublicPlans(const TArray<int> PlanId, const TArray<FString>& PlanExternalId, const FString& Country, const FString& Locale,
		const FOnSubscriptionPublicPlansListUpdate& SuccessCallback, const FOnError& ErrorCallback,	const int Limit = 50, const int Offset = 0);

	/** Returns a list of plans available to authorized users, including plans purchased by the user while promotions are active.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscriptions-purchase/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param PlanId Array of subscription plan IDs. Plan ID can be found in the URL of the subscription details page in Publisher Account (https://publisher.xsolla.com/{merchant_id}/projects/{project_id}/subscriptions/plans/{merplan_id}).
	* @param PlanExternalId Array of subscription plan external IDs (32 characters per ID). Plan external ID can be found in Publisher Account in the **Subscriptions > Subscription plans** section next to the plan name.
	* @param Country User's country. Affects the choice of locale and currency. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). By default, it is determined by the user's IP address.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address.<br>
	* Can be enforced by using language code: The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`).
	* @param SuccessCallback Called after a list of plans has been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page (15 elements are displayed by default).
	* @param Offset Number of elements from which the list is generated (the count starts from 0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "PlanId, PlanExternalId, SuccessCallback, ErrorCallback"))
	void GetSubscriptionPlans(const FString& AuthToken, const TArray<int> PlanId, const TArray<FString>& PlanExternalId, const FString& Country, const FString& Locale,
		const FOnSubscriptionPlansListUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns a list of active recurrent subscriptions that have the status `active`, `non renewing`, and `pause`.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscription-management/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param Locale Language of the UI. By default, it is determined by the user's IP address.<br>
	* Can be enforced by using language code: The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`).
	* @param SuccessCallback Called after the list pf subscriptions has been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param Limit Limit for the number of elements on the page (15 elements are displayed by default).
	* @param Offset Number of elements from which the list is generated (the count starts from 0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptions(const FString& AuthToken, const FString& Locale,
		const FOnSubscriptionsListUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Limit = 50, const int Offset = 0);

	/** Returns information about a subscription by its ID. Subscription can be have any status.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscription-management/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param SubscriptionId Subscription ID.
	* @param Locale Language of the UI. By default, it is determined by the user's IP address.<br>
	* Can be enforced by using language code: The following languages are supported: Arabic (`ar`), Bulgarian (`bg`), Czech (`cs`), German (`de`), Spanish (`es`), French (`fr`), Hebrew (`he`), Italian (`it`), Japanese (`ja`), Korean (`ko`), Polish (`pl`), Portuguese (`pt`), Romanian (`ro`), Russian (`ru`), Thai (`th`), Turkish (`tr`), Vietnamese (`vi`), Chinese Simplified (`cn`), Chinese Traditional (`tw`), English (`en`).
	* @param SuccessCallback Called after subscription data have been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionDetails(const FString& AuthToken, const int32 SubscriptionId, const FString& Locale,
		const FOnGetSubscriptionDetailsSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns Pay Station URL for the subscription purchase.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscriptions-purchase/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param PlanExternalId Subscription plan external ID (32 characters). Plan external ID can be found in Publisher Account in the **Subscriptions > Subscription plans** section next to the plan name.
	* @param Country User's country. Affects the choice of locale and currency. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). By default, it is determined by the user's IP address.
	* @param SuccessCallback Called after the URL has been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param bShowCloseButton (optional) Whether to show the ← icon in Pay Station so the user can close the payment UI at any stage of the purchase.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionPurchaseUrl(const FString& AuthToken, const FString& PlanExternalId, const FString& Country,
		const FOnGetSubscriptionPayStationLinkSuccess& SuccessCallback, const FOnError& ErrorCallback, const bool bShowCloseButton = false);

	/** Returns the URL of the management interface for the selected subscription.
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscription-management/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param Country User's country. Two-letter uppercase country code per [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). Affects the choice of locale and currency. By default, it is determined by the user's IP address.
	* @param SuccessCallback Called after the URL has been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param bShowCloseButton (optional) Whether to show the ← icon in Pay Station so the user can close the payment UI at any stage of the purchase.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionManagementUrl(const FString& AuthToken, const FString& Country,
		const FOnGetSubscriptionPayStationLinkSuccess& SuccessCallback, const FOnError& ErrorCallback, const bool bShowCloseButton = false);

	/** Returns the URL of the renewal interface for the selected subscription
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscription-management/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param SubscriptionId Subscription ID.
	* @param SuccessCallback Called after the URL has been successfully recieved.
	* @param ErrorCallback Called after the request resulted with an error.
	* @param bShowCloseButton (optional) Whether to show the ← icon in Pay Station so the user can close the payment UI at any stage of the purchase.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Subscriptions", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSubscriptionRenewalUrl(const FString& AuthToken, const int32 SubscriptionId,
		const FOnGetSubscriptionPayStationLinkSuccess& SuccessCallback, const FOnError& ErrorCallback, const bool bShowCloseButton = false);

	/** Changes a regular subscription status to `non_renewing` (subscription is automatically canceled after expiration).
	* [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/subscriptions/subscription-management/).
	*
	* @param AuthToken User authorization token obtained during authorization using Xsolla Login ([more about authorization options](https://developers.xsolla.com/sdk/unreal-engine/authentication/)).
	* @param SubscriptionId Subscription ID.
	* @param SuccessCallback Called after successful subscription cancelling.
	* @param ErrorCallback Called after the request resulted with an error.
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
	void CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnCheckOrder SuccessCallback, FOnError ErrorCallback);

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

	void InnerPurchase(const FString& AuthToken, const FString& Sku, bool bIsFree, const TArray<FXsollaVirtualCurrencyPrice>& VirtualPrices,
		const FXsollaPaymentTokenRequestPayload PaymentTokenRequestPayload, const FOnPurchaseUpdate& SuccessCallback, const FOnError& ErrorCallback);

	UFUNCTION()
	void FetchTokenCallback(const FString& AccessToken, int32 InOrderId);

	UFUNCTION()
	void BuyVirtualOrFreeItemCallback(int32 InOrderId);

	UFUNCTION()
	void CheckPendingOrderSuccessCallback();

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET,
		const FString& AuthToken = FString(), const FString& Content = FString());

	/** Serialize json object into string */
	FString SerializeJson(const TSharedPtr<FJsonObject> DataJson) const;

	/** Try to execute next request in queue */
	void ProcessNextCartRequest();

	/** Prepare payload for payment token request */
	TSharedPtr<FJsonObject> PreparePaymentTokenRequestPayload(const FXsollaPaymentTokenRequestPayload& PaymentTokenRequestPayload);

	/** Prepare paystation settings */
	TSharedPtr<FJsonObject> PreparePaystationSettings(const bool bShowCloseButton);

	/** Extract Steam user ID from auth token */
	bool GetSteamUserId(const FString& AuthToken, FString& SteamId, FString& OutError);

	/** Queue to store cart change requests */
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> CartRequestsQueue;

public:
	/** Returns the list of cached virtual items without any Category provided. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	TArray<FStoreItem> GetVirtualItemsWithoutGroup() const;

	/** Returns cached items data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FStoreItemsData& GetItemsData() const;

	/** Returns cached cart data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart")
	const FStoreCart& GetCart() const;

	/** Returns the pending PayStation URL to be opened in browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FString& GetPendingPaystationUrl() const;

	/** Returns name of the cached item with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FString GetItemName(const FString& ItemSKU) const;

	/** Returns item from the cache with the given SKU. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	const FStoreItem& FindItemBySku(const FString& ItemSku, bool& bHasFound) const;

	/** Returns package from the cache with the given SKU. */
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
	FString PendingPaystationUrl;

	UXsollaStoreBrowserWrapper* MyBrowser;

private:
	UPROPERTY()
	TSubclassOf<UXsollaStoreBrowserWrapper> DefaultBrowserWidgetClass;

	UPROPERTY()
	TArray<UXsollaOrderCheckObject*> CachedOrderCheckObjects;

	UPROPERTY()
	UXsollaLoginSubsystem* LoginSubsystem;

	UPROPERTY()
	FString PaymentAccessToken;

	UPROPERTY()
	FString PaymentRedirectURI;

	UPROPERTY()
	bool PaymentEnableSandbox;

	UPROPERTY()
	int32 PaymentOrderId;

	FOnPurchaseUpdate PaymentSuccessCallback;

	FOnError PaymentErrorCallback;

	UPROPERTY()
	FOnStoreBrowserClosed PaymentBrowserClosedCallback;
};
