// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Brushes/SlateDynamicImageBrush.h"
#include "CoreMinimal.h"
#include "Http.h"

#include "XsollaStoreImageLoader.generated.h"

//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnImageLoaded, FSlateBrush, ImageBrush);

UCLASS()
class XSOLLASTORE_API UXsollaStoreImageLoader : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback"))
	void LoadImage(FString URL, const FOnImageLoaded& SuccessCallback);

protected:
	/** */
	void LoadImage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnImageLoaded SuccessCallback);
};
