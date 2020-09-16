// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaUtilsLibrary.generated.h"

class UXsollaUtilsImageLoader;

UCLASS()
class XSOLLAUTILS_API UXsollaUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to image loader object */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Utils")
	static UXsollaUtilsImageLoader* GetImageLoader();
};
