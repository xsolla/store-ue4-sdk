// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUtilsWidgetsLibrary.generated.h"

UCLASS(abstract, Blueprintable, BlueprintType)
class UXsollaUtilsWidgetsLibrary : public UObject
{
	GENERATED_BODY()

public:
	
	/** Get theme brush with specified name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla Widgets Library")
	TSubclassOf<UUserWidget> GetWidget(const FString& WidgetName) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla Widgets Library")
	TMap<FString, TSubclassOf<UUserWidget>> Widgets;
};
