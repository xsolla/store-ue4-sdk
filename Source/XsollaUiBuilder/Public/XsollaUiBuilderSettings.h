// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ThemeObject.h"
#include "XsollaUiBuilderDataModel.h"

#include "XsollaUiBuilderSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLAUIBUILDER_API UXsollaUiBuilderSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	int CurrentThemeIndex;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	bool bUseThemeObject;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TSubclassOf<UThemeObject> CurrentTheme;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UI Builder Customization")
	TArray<FTheme> Themes;
};
