// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "XsollaUtilsTheme.generated.h"

UCLASS(abstract, Blueprintable, BlueprintType)
class UXsollaUtilsTheme : public UObject
{
	GENERATED_BODY()

public:
	/** Get theme color with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FColor GetColor(const FString& ColorName) const;
	
	/** Get theme font with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateFontInfo GetFont(const FString& FontName) const;
	
	/** Get theme brush with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateBrush GetBrush(const FString& BrushName) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<FString, FColor> Colors;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<FString, FSlateFontInfo> Fonts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<FString, FSlateBrush> Brushes;
};
