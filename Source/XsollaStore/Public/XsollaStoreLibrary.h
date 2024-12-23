// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaStoreDataModel.h"
#include "XsollaStoreLibrary.generated.h"

class UXsollaStoreSubsystem;
class UDataTable;

UCLASS()
class XSOLLASTORE_API UXsollaStoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns true if cart A is equal to cart B (A == B). */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (StoreCart)", CompactNodeTitle = "===", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"), Category = "Xsolla|Store|Cart")
	static bool Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B);

	/** Returns currency-format data table. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store")
	static UDataTable* GetCurrencyLibrary();

	/** Formats store price using the currency-format library https://github.com/xsolla/currency-format. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Store")
	static FString FormatPrice(float Amount, const FString& Currency = TEXT("USD"));

	/** Convert FStoreBundle to FStoreItem */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To StoreItem (StoreBundle)", CompactNodeTitle = "->", ScriptMethod = "StoreItem", Keywords = "cast convert", BlueprintAutocast), Category = "Xsolla|Store|Conversions")
	static FStoreItem Conv_StoreBundleToStoreItem(FStoreBundle InStoreBundle);

	/** Convert FStoreItem to FStoreCartItem */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To StoreCartItem (StoreItem)", CompactNodeTitle = "->", ScriptMethod = "StoreCartItem", Keywords = "cast convert", BlueprintAutocast), Category = "Xsolla|Store|Conversions")
	static FStoreCartItem Conv_StoreItemToStoreCartItem(FStoreItem InStoreItem);

private:
	/** Loaded currency library asset. */
	static UDataTable* CurrencyLibrary;
};
