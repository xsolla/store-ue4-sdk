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

	/** Make FDateTime structure based on a given timestamp */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Store")
    static FDateTime MakeDateTimeFromTimestamp(int64 Time);
};
