// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Widgets/XsollaGenericPrimitive.h"
#include "XsollaUIBuilderTypes.h"

#include "XsollaUIBuilderWidgetsLibrary.generated.h"

UCLASS(abstract, Blueprintable, BlueprintType)
class UXsollaUIBuilderWidgetsLibrary : public UObject
{
	GENERATED_BODY()

public:
	/** Get widget class. */
	UFUNCTION(BlueprintPure, Category = "Xsolla Widgets Library")
	TSubclassOf<UXsollaGenericPrimitive> GetWidgetByType(const TEnumAsByte<EWidgetType> WidgetType) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Xsolla Widgets Library")
	TMap<TEnumAsByte<EWidgetType>, TSubclassOf<UXsollaGenericPrimitive>> Widgets;
};
