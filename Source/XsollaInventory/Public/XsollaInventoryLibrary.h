// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaInventoryLibrary.generated.h"

class UXsollaInventorySettings;

UCLASS()
class XSOLLAINVENTORY_API UXsollaInventoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to inventory SDK settings */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Inventory")
	static UXsollaInventorySettings* GetInventorySettings();
};
