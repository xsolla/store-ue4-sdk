// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreController.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaStoreLibrary.generated.h"

UCLASS()
class XSOLLASTORE_API UXsollaStoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to Store controller */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Store", meta = (WorldContext = "WorldContextObject"))
	static UXsollaStoreController* GetStoreController(UObject* WorldContextObject);

	/** Update list of virtual items */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback", WorldContext = "WorldContextObject"))
	static void UpdateVirtualItems(UObject* WorldContextObject, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback);
};
