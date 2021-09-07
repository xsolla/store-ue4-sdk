// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaUIBuilderTypes.generated.h"

USTRUCT(BlueprintType)
struct FEntityTypeName
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TypeAsInt;

	UPROPERTY()
	FName Name;

	FEntityTypeName()
		: TypeAsInt(INT32_MAX)
	{
	}

	FEntityTypeName(int32 InTypeAsInt, const FName& InName)
		: TypeAsInt(InTypeAsInt)
		, Name(InName)
	{
	}
};

UENUM(BlueprintType)
enum EWidgetType
{
	WidgetType_Default UMETA(DisplayName = "Default"),
	WidgetType1 UMETA(Hidden),
	WidgetType2 UMETA(Hidden),
	WidgetType3 UMETA(Hidden),
	WidgetType4 UMETA(Hidden),
	WidgetType5 UMETA(Hidden),
	WidgetType6 UMETA(Hidden),
	WidgetType7 UMETA(Hidden),
	WidgetType8 UMETA(Hidden),
	WidgetType9 UMETA(Hidden),
	WidgetType10 UMETA(Hidden),
	WidgetType11 UMETA(Hidden),
	WidgetType12 UMETA(Hidden),
	WidgetType13 UMETA(Hidden),
	WidgetType14 UMETA(Hidden),
	WidgetType15 UMETA(Hidden),
	WidgetType16 UMETA(Hidden),
	WidgetType17 UMETA(Hidden),
	WidgetType18 UMETA(Hidden),
	WidgetType19 UMETA(Hidden),
	WidgetType20 UMETA(Hidden),
	WidgetType21 UMETA(Hidden),
	WidgetType22 UMETA(Hidden),
	WidgetType23 UMETA(Hidden),
	WidgetType24 UMETA(Hidden),
	WidgetType25 UMETA(Hidden),
	WidgetType26 UMETA(Hidden),
	WidgetType27 UMETA(Hidden),
	WidgetType28 UMETA(Hidden),
	WidgetType29 UMETA(Hidden),
	WidgetType30 UMETA(Hidden),
	WidgetType_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum EColorType
{
	ColorType_Default UMETA(DisplayName = "Default"),
	ColorType1 UMETA(Hidden),
	ColorType2 UMETA(Hidden),
	ColorType3 UMETA(Hidden),
	ColorType4 UMETA(Hidden),
	ColorType5 UMETA(Hidden),
	ColorType6 UMETA(Hidden),
	ColorType7 UMETA(Hidden),
	ColorType8 UMETA(Hidden),
	ColorType9 UMETA(Hidden),
	ColorType10 UMETA(Hidden),
	ColorType11 UMETA(Hidden),
	ColorType12 UMETA(Hidden),
	ColorType13 UMETA(Hidden),
	ColorType14 UMETA(Hidden),
	ColorType15 UMETA(Hidden),
	ColorType16 UMETA(Hidden),
	ColorType17 UMETA(Hidden),
	ColorType18 UMETA(Hidden),
	ColorType19 UMETA(Hidden),
	ColorType20 UMETA(Hidden),
	ColorType21 UMETA(Hidden),
	ColorType22 UMETA(Hidden),
	ColorType23 UMETA(Hidden),
	ColorType24 UMETA(Hidden),
	ColorType25 UMETA(Hidden),
	ColorType26 UMETA(Hidden),
	ColorType27 UMETA(Hidden),
	ColorType28 UMETA(Hidden),
	ColorType29 UMETA(Hidden),
	ColorType30 UMETA(Hidden),
	ColorType_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum EFontType
{
	FontType_Default UMETA(DisplayName = "Default"),
	FontType1 UMETA(Hidden),
	FontType2 UMETA(Hidden),
	FontType3 UMETA(Hidden),
	FontType4 UMETA(Hidden),
	FontType5 UMETA(Hidden),
	FontType6 UMETA(Hidden),
	FontType7 UMETA(Hidden),
	FontType8 UMETA(Hidden),
	FontType9 UMETA(Hidden),
	FontType10 UMETA(Hidden),
	FontType11 UMETA(Hidden),
	FontType12 UMETA(Hidden),
	FontType13 UMETA(Hidden),
	FontType14 UMETA(Hidden),
	FontType15 UMETA(Hidden),
	FontType16 UMETA(Hidden),
	FontType17 UMETA(Hidden),
	FontType18 UMETA(Hidden),
	FontType19 UMETA(Hidden),
	FontType20 UMETA(Hidden),
	FontType21 UMETA(Hidden),
	FontType22 UMETA(Hidden),
	FontType23 UMETA(Hidden),
	FontType24 UMETA(Hidden),
	FontType25 UMETA(Hidden),
	FontType26 UMETA(Hidden),
	FontType27 UMETA(Hidden),
	FontType28 UMETA(Hidden),
	FontType29 UMETA(Hidden),
	FontType30 UMETA(Hidden),
	FontType_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum EBrushThemeType
{
	BrushType_Default UMETA(DisplayName = "Default"),
	BrushType1 UMETA(Hidden),
	BrushType2 UMETA(Hidden),
	BrushType3 UMETA(Hidden),
	BrushType4 UMETA(Hidden),
	BrushType5 UMETA(Hidden),
	BrushType6 UMETA(Hidden),
	BrushType7 UMETA(Hidden),
	BrushType8 UMETA(Hidden),
	BrushType9 UMETA(Hidden),
	BrushType10 UMETA(Hidden),
	BrushType11 UMETA(Hidden),
	BrushType12 UMETA(Hidden),
	BrushType13 UMETA(Hidden),
	BrushType14 UMETA(Hidden),
	BrushType15 UMETA(Hidden),
	BrushType16 UMETA(Hidden),
	BrushType17 UMETA(Hidden),
	BrushType18 UMETA(Hidden),
	BrushType19 UMETA(Hidden),
	BrushType20 UMETA(Hidden),
	BrushType21 UMETA(Hidden),
	BrushType22 UMETA(Hidden),
	BrushType23 UMETA(Hidden),
	BrushType24 UMETA(Hidden),
	BrushType25 UMETA(Hidden),
	BrushType26 UMETA(Hidden),
	BrushType27 UMETA(Hidden),
	BrushType28 UMETA(Hidden),
	BrushType29 UMETA(Hidden),
	BrushType30 UMETA(Hidden),
	BrushType31 UMETA(Hidden),
	BrushType32 UMETA(Hidden),
	BrushType33 UMETA(Hidden),
	BrushType34 UMETA(Hidden),
	BrushType35 UMETA(Hidden),
	BrushType36 UMETA(Hidden),
	BrushType37 UMETA(Hidden),
	BrushType38 UMETA(Hidden),
	BrushType39 UMETA(Hidden),
	BrushType40 UMETA(Hidden),
	BrushType41 UMETA(Hidden),
	BrushType42 UMETA(Hidden),
	BrushType43 UMETA(Hidden),
	BrushType44 UMETA(Hidden),
	BrushType45 UMETA(Hidden),
	BrushType46 UMETA(Hidden),
	BrushType47 UMETA(Hidden),
	BrushType48 UMETA(Hidden),
	BrushType49 UMETA(Hidden),
	BrushType50 UMETA(Hidden),
	BrushType51 UMETA(Hidden),
	BrushType52 UMETA(Hidden),
	BrushType53 UMETA(Hidden),
	BrushType54 UMETA(Hidden),
	BrushType55 UMETA(Hidden),
	BrushType56 UMETA(Hidden),
	BrushType57 UMETA(Hidden),
	BrushType58 UMETA(Hidden),
	BrushType59 UMETA(Hidden),
	BrushType60 UMETA(Hidden),
	BrushType61 UMETA(Hidden),
	BrushType62 UMETA(Hidden),
	BrushType_Max UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FThemeParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XSolla|UIBuilderTypes")
	TMap<FString, TEnumAsByte<EColorType>> Colors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XSolla|UIBuilderTypes")
	TMap<FString, TEnumAsByte<EFontType>> Fonts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XSolla|UIBuilderTypes")
	TMap<FString, TEnumAsByte<EBrushThemeType>> Brushes;
};