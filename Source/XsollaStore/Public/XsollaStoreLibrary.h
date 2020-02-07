// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaStoreDataModel.h"

#include "XsollaStoreLibrary.generated.h"

class UXsollaStoreSubsystem;

UCLASS()
class XSOLLASTORE_API UXsollaStoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns true if cart A is equal to cart B (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (StoreCart)", CompactNodeTitle = "===", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"), Category = "Xsolla|Store|Cart")
	static bool Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B);
};
