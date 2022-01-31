// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaSettingsLibrary.generated.h"

class UXsollaSettings;

/**
 * 
 */
UCLASS()
class XSOLLASETTINGS_API UXsollaSettingsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Gives direct access to the SDK settings. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Settings")
	static UXsollaSettings* GetXsollaSettings();
	
};
