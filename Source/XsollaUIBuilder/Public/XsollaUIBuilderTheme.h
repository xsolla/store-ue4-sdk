// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "XsollaUIBuilderTypes.h"

#include "XsollaUIBuilderTheme.generated.h"

UCLASS(abstract, Blueprintable, BlueprintType)
class UXsollaUIBuilderTheme : public UObject
{
	GENERATED_BODY()

public:
	/** Get theme color with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FColor GetColorByType(const EColorType ColorType, bool& bIsFound) const;

	/** Get theme font with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateFontInfo GetFontByType(const EFontType FontType, bool& bIsFound) const;

	/** Get theme brush with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla UI Theme")
	FSlateBrush GetBrushByType(const EBrushThemeType BrushType, bool& bIsFound) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<TEnumAsByte<EColorType>, FColor> ColorsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<TEnumAsByte<EFontType>, FSlateFontInfo> FontsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla UI Theme")
	TMap<TEnumAsByte<EBrushThemeType>, FSlateBrush> BrushesMap;
};
