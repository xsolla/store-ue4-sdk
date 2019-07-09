// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreImageLoader.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaStoreLibrary.generated.h"

class UXsollaStoreController;
struct FStoreCart;

UCLASS()
class XSOLLASTORE_API UXsollaStoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to Store controller */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Store", meta = (WorldContext = "WorldContextObject"))
	static UXsollaStoreController* GetStoreController(UObject* WorldContextObject);

	/** Async load image from web */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (WorldContext = "WorldContextObject"))
	static void LoadImageFromWeb(UObject* WorldContextObject, const FString& URL, const FOnImageLoaded& SuccessCallback);

	/** Returns true if cart A is equal to cart B (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (StoreCart)", CompactNodeTitle = "===", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"), Category = "Xsolla|Store|Cart")
	static bool Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B);
};
