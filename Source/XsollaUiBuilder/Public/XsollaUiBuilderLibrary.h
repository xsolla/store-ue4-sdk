// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "XsollaUiBuilderSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaUiBuilderLibrary.generated.h"

UCLASS()
class XSOLLAUIBUILDER_API UXsollaUiBuilderLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "XsollaUiBuilderLibrary")
	static UXsollaUiBuilderSettings* GetUiBuilderSettings();

	UFUNCTION(BlueprintPure, Category = "XsollaUiBuilderLibrary")
	static FColor GetUiBuilderColorByName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "XsollaUiBuilderLibrary")
    static UClass* GetUiBuilderWidgetByName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "XsollaUiBuilderLibrary")
    static FSlateFontInfo GetUIBuilderFontByName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "XsollaUiBuilderLibrary")
    static FSlateBrush GetUIBuilderBrushByName(const FString& Name, bool& bIsFound);

	UFUNCTION(BlueprintPure, Category = "XsollaUiBuilderLibrary")
    static FTheme GetUiBuilderCurrentTheme();
};
