// Copyright 2021 Xsolla Inc. All Rights Reserved.
#pragma once

#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "XsollaUiBuilderDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLAUIBUILDER_API FTheme
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TMap<FString, UClass*> Widgets;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TMap<FString, FColor> Colors;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TMap<FString, FSlateFontInfo> Fonts;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TMap<FString, FSlateBrush> Brushes;
};