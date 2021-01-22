// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThemeObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class XSOLLAUIBUILDER_API UThemeObject : public UObject
{
	GENERATED_BODY()

	static UThemeObject* ThemeInstance;
	
	static TSubclassOf<UThemeObject> ThemeInstanceClass;

	static void NewThemeInstance();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TMap<FString, UClass*> Widgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TMap<FString, FColor> Colors;

	static UThemeObject* GetThemeInstance();
};
