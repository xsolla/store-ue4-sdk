// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaSettingsLibrary.generated.h"

class UXsollaProjectSettings;

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
	static UXsollaProjectSettings* GetProjectSettings();
	
};
