// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaUtilsTheme.generated.h"

UCLASS(abstract, Blueprintable, BlueprintType)
class UXsollaUtilsTheme : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Get theme color with specified name. */
	UFUNCTION(BlueprintPure)
	FColor GetColor(const FString& ColorName) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FColor> Colors;
};
