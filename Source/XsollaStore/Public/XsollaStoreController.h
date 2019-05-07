// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Http.h"

#include "XsollaStoreController.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnStoreUpdate);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnStoreError, const FString&, Code, const FString&, Description);

UCLASS()
class XSOLLASTORE_API UXsollaStoreController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);

protected:
	void UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback);

	/** Return true if error is happened */
	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreError ErrorCallback);
};
