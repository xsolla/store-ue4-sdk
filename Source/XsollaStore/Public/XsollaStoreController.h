// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Http.h"

#include "XsollaStoreController.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnStoreUpdate);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnStoreError, int32, StatusCode, int32, ErrorCode, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFetchTokenSuccess, const FString&, AccessToken);

UCLASS()
class XSOLLASTORE_API UXsollaStoreController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Update list of virtual items */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

	/**
	 * Initiate item purchase session and fetch token for payment console
	 *
	 * @param AuthToken User authorization token.
	 * @param ItemSKU Desired item SKU.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback);

	/** Open payment console for provided access token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	void LaunchPaymentConsole(const FString& AccessToken /** @TODO Add callbacks to control payment progress */);

protected:
	void UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);
	void FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback);

	/** Return true if error is happened */
	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreError ErrorCallback);

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url);
};
