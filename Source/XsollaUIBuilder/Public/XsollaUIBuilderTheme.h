// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaUIBuilderTypes.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "XsollaUIBuilderTheme.generated.h"

UCLASS(abstract, Blueprintable, BlueprintType)
class UXsollaUIBuilderTheme : public UObject
{
	GENERATED_BODY()

public:
	/** Get theme color with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FColor GetColor(const FString& ColorName) const;

	/** Get theme color with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FColor GetColorByType(const EColorType ColorType) const;
	
	/** Get theme font with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateFontInfo GetFont(const FString& FontName) const;

	/** Get theme font with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateFontInfo GetFontByType(const EFontType FontType) const;
	
	/** Get theme brush with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateBrush GetBrush(const FString& BrushName) const;

	/** Get theme brush with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateBrush GetBrushByType(const EBrushThemeType BrushType) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<FString, FColor> Colors;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<FString, FSlateFontInfo> Fonts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<FString, FSlateBrush> Brushes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<TEnumAsByte<EColorType>, FColor> ColorsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<TEnumAsByte<EFontType>, FSlateFontInfo> FontsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<TEnumAsByte<EBrushThemeType>, FSlateBrush> BrushesMap;
};
