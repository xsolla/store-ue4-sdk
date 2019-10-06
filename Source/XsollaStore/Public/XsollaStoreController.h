// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"

#include "Blueprint/UserWidget.h"
#include "Http.h"

#include "XsollaStoreController.generated.h"

class UXsollaStoreImageLoader;

class UDataTable;

DECLARE_DYNAMIC_DELEGATE(FOnStoreUpdate);
DECLARE_DYNAMIC_DELEGATE(FOnStoreCartUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCartUpdate, const FStoreCart&, Cart);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnStoreError, int32, StatusCode, int32, ErrorCode, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchTokenSuccess, const FString&, AccessToken, int32, OrderId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCheckOrder, int32, OrderId, EXsollaOrderStatus, OrderStatus);

UCLASS()
class XSOLLASTORE_API UXsollaStoreController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
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
	void UpdateInventory(const FString& AuthToken, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

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
	 * @param Currency (optional) Desired payment currency. Leave empty to use default value.
	 * @param Country (optional) Desired payment country ISO code. Leave empty to use default value.
	 * @param Locale (optional) Desired payment locale. Leave empty to use default value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchCartPaymentToken(const FString& AuthToken, const FString& Currency, const FString& Country, const FString& Locale, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Open payment console for provided access token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void LaunchPaymentConsole(const FString& AccessToken, UUserWidget*& BrowserWidget);

	/** Check pending order status */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckOrder(const FString& AuthToken, int32 OrderId, const FOnCheckOrder& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Create new cart */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Remove all items from cart  */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ClearCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Update cart content */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Add item to cart and change its quantity */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AddToCart(const FString& AuthToken, const FString& ItemSKU, int32 Quantity, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Remove item from cart */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Cart", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveFromCart(const FString& AuthToken, const FString& ItemSKU, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Consume inventory item
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 * @param Quantity Items quantity. If item is uncountable, should be zero.
	 * @param InstanceID Instance item ID. If item is countable, should be empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Inventory", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU, int32 Quantity, const FString& InstanceID, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

protected:
	void UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateItemGroups_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateInventory_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback);
	void CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckOrder SuccessCallback, FOnStoreError ErrorCallback);

	void CreateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void ClearCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void UpdateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void AddToCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void RemoveFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback);

	void ConsumeInventoryItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);

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
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url);

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

	/** User inventory */
	FStoreInventory Inventory;

	/** Cached cart desired currency (used for silent cart update) */
	FString CachedCartCurrency;

	/** Cached auth token (used for silent cart update) */
	FString CachedAuthToken;

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
};
