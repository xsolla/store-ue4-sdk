// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Brushes/SlateDynamicImageBrush.h"
#include "CoreMinimal.h"
#include "Http.h"
#include "XsollaUtilsImageLoader.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnImageLoaded, FSlateBrush, ImageBrush, FString, ImageURL);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnImageLoadFailed, FString, ImageURL);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRequestCompleted, bool);

/**
 * Async image loading from web. Should be used for DEMO PUPPOSES ONLY.
 */
UCLASS()
class XSOLLAUTILS_API UXsollaUtilsImageLoader : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Utils", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LoadImage(const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback);

protected:
	/** */
	void LoadImage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnImageLoaded SuccessCallback, FOnImageLoadFailed ErrorCallback);

private:
	FName GetCacheName(const FString& URL) const;

	/** Internal brushes cache */
	TMap<FString, TSharedPtr<FSlateDynamicImageBrush>> ImageBrushes;

	/** Internal cache for pending requests callbacks */
	TMap<FString, FOnRequestCompleted> PendingRequests;
};
