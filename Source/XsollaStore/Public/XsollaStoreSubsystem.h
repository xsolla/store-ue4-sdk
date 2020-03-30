// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreImageLoader.h"

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

#include "XsollaStoreSubsystem.generated.h"

/** Verb (GET, PUT, POST) used by the request */
UENUM(BlueprintType)
enum class EXsollaRequestVerb : uint8
{
	GET,
	POST,
	PUT,
	DELETE
};

class UXsollaStoreImageLoader;
class UDataTable;
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

	/** Initialize controller with provided project id (use to override project settings) */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void Initialize(const FString& InProjectId);

	/** Update list of virtual items */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update list of item groups */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateItemGroups(const FString& Locale, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update list of purchased virtual items */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateInventory(const FString& AuthToken, const FString& Platform, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update list of virtual currencies */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualCurrencies(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update list of virtual currency packages */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualCurrencyPackages(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update virtual currency balance */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualCurrencyBalance(const FString& AuthToken, const FString& Platform, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Initiate item purchase session and fetch token for payment console
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param Currency (optional) Desired payment currency. Leave empty to use default value.
	 * @param Country (optional) Desired payment country ISO code. Leave empty to use default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use default value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU, const FString& Currency, const FString& Country, const FString& Locale, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Initiate cart purchase session and fetch token for payment console
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart to be purchased. Current user cart will be purchased if empty.
	 * @param Currency (optional) Desired payment currency. Leave empty to use default value.
	 * @param Country (optional) Desired payment country ISO code. Leave empty to use default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use default value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchCartPaymentToken(const FString& AuthToken, const FString& CartId, const FString& Currency, const FString& Country, const FString& Locale, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Open payment console for provided access token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void LaunchPaymentConsole(const FString& AccessToken, UUserWidget*& BrowserWidget);

	/** Check pending order status */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckOrder(const FString& AuthToken, int32 OrderId, const FOnCheckOrder& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Create new cart */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (DeprecatedFunction, AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Remove all items from cart
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart to be cleared. Current user cart will be cleared if empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ClearCart(const FString& AuthToken, const FString& CartId, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Update cart content
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart to be updated. Current user cart will be updated if empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateCart(const FString& AuthToken, const FString& CartId, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Add item to cart and change its quantity
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart to which item will be added. Current user cart will be modified if empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AddToCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, int32 Quantity, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Remove item from cart
	 *
	 * @param AuthToken User authorization token.
	 * @param CartId (optional) Identifier of cart from which item will be removed. Current user cart will be modified if empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveFromCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Consume inventory item
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Items quantity. If item is uncountable, should be zero.
	 * @param InstanceID Instance item ID. If item is countable, should be empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Inventory", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ConsumeInventoryItem(const FString& AuthToken, const FString& Platform, const FString& ItemSKU, int32 Quantity, const FString& InstanceID, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Get virtual currency with specified SKU */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetVirtualCurrency(const FString& CurrencySKU, const FOnCurrencyUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Get virtual currency package with specified SKU */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetVirtualCurrencyPackage(const FString& PackageSKU, const FOnCurrencyPackageUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Buy item using virtual currency */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void BuyItemWithVirtualCurrency(const FString& AuthToken, const FString& ItemSKU, const FString& CurrencySKU, const FOnPurchaseUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

protected:
	void UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateItemGroups_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateInventory_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateVirtualCurrencies_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateVirtualCurrencyPackages_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateVirtualCurrencyBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback);
	void CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckOrder SuccessCallback, FOnStoreError ErrorCallback);

	void CreateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void ClearCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void AddToCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void RemoveFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void ConsumeInventoryItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void GetVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCurrencyUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void GetVirtualCurrencyPackage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCurrencyPackageUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void BuyItemWithVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FOnStoreError ErrorCallback);

	/** Return true if error is happened */
	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreError ErrorCallback);

protected:
	/** Load save game and extract data */
	void LoadData();

	/** Save cached data or reset one if necessary */
	void SaveData();

	/** Check whether sandbox is enabled */
	bool IsSandboxEnabled() const;

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url, const EXsollaRequestVerb Verb = EXsollaRequestVerb::GET, const FString& AuthToken = FString(), const FString& Content = FString());

	/** Serialize json object into string */
	FString SerializeJson(const TSharedPtr<FJsonObject> DataJson) const;

	/** Parse JWT token and get its payload as json object */
	bool ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject) const;

	/** Try to execute next request in queue */
	void ProcessNextCartRequest();

	/** Queue to store cart change requests */
	TArray<TSharedRef<IHttpRequest>> CartRequestsQueue;

public:
	/** Get list of cached virtual items filtered by Category */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	TArray<FStoreItem> GetVirtualItems(const FString& GroupFilter) const;

	/** Get list of cached virtual items without any Category provided */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	TArray<FStoreItem> GetVirtualItemsWithoutGroup() const;

	/** Get cached items data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FStoreItemsData GetItemsData() const;

	/** Get list of cached virtual currencies */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency")
	TArray<FVirtualCurrency> GetVirtualCurrencyData() const;

	/** Get list of cached virtual currency packages */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency")
	TArray<FVirtualCurrencyPackage> GetVirtualCurrencyPackages() const;

	/** Get cached virtual currencies balance */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|VirtualCurrency")
	TArray<FVirtualCurrencyBalance> GetVirtualCurrencyBalance() const;

	/** Get cached cart data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart")
	FStoreCart GetCart() const;

	/** Get cached inventory data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Inventory")
	FStoreInventory GetInventory() const;

	/** Get pending paystation url to be opened in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	FString GetPendingPaystationUrl() const;

	/** Get currency-format data table */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	UDataTable* GetCurrencyLibrary() const;

public:
	/** Event occured when the cart was changed or updated */
	UPROPERTY(BlueprintAssignable, Category = "Xsolla|Store|Cart")
	FOnCartUpdate OnCartUpdate;

protected:
	/** Cached Xsolla Store project id */
	FString ProjectId;

	/** Cached virtual items list */
	FStoreItemsData ItemsData;

	/** Current cart */
	FStoreCart Cart;

	/** Cached list of virtual currencies */
	FVirtualCurrencyData VirtualCurrencyData;

	/** Cached virtual currency packages */
	FVirtualCurrencyPackagesData VirtualCurrencyPackages;

	/** Cached virtual currency balance */
	FVirtualCurrencyBalanceData VirtualCurrencyBalance;

	/** User inventory */
	FStoreInventory Inventory;

	/** Cached cart desired currency (used for silent cart update) */
	FString CachedCartCurrency;

	/** Cached auth token (used for silent cart update) */
	FString CachedAuthToken;

	/** Cached cart identifier (used for silent cart update) */
	FString CachedCartId;

	/** Pending paystation url to be opened in browser */
	FString PengindPaystationUrl;

	/** Loaded currency library asset */
	UPROPERTY(BlueprintReadOnly, Category = "Xsolla|Currency")
	UDataTable* CurrencyLibrary;

public:
	UXsollaStoreImageLoader* GetImageLoader() const;

private:
	UPROPERTY()
	UXsollaStoreImageLoader* ImageLoader;

	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;

public:
	/** Async load image from web */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "ErrorCallback"))
	void LoadImageFromWeb(const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback);

	/** Format store price using currency-format library https://github.com/xsolla/currency-format */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Store")
	FString FormatPrice(float Amount, const FString& Currency = TEXT("USD")) const;
};
