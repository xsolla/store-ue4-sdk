// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUIBuilderTheme.h"
#include "XsollaUIBuilderWidgetsLibrary.h"

#include "XsollaUIBuilderSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLAUIBUILDER_API UXsollaUIBuilderSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** Interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UIBuilder Settings")
	TSubclassOf<UXsollaUIBuilderTheme> InterfaceTheme;

	/* Widgets library. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UIBuilder Settings")
	TSubclassOf<UXsollaUIBuilderWidgetsLibrary> WidgetsLibrary;
};
