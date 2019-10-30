// Copyright 2019 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaPayStationLibrary.generated.h"

class UXsollaPayStationController;
class UXsollaPayStationSettings;

UCLASS()
class XSOLLAPAYSTATION_API UXsollaPayStationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to PayStation controller */
	UFUNCTION(BlueprintPure, Category = "Xsolla|PayStation", meta = (WorldContext = "WorldContextObject"))
	static UXsollaPayStationController* GetPayStationController(UObject* WorldContextObject);

	/** Direct access to PayStation SDK settings */
	UFUNCTION(BlueprintPure, Category = "Xsolla|PayStation")
	static UXsollaPayStationSettings* GetPayStationSettings();
};