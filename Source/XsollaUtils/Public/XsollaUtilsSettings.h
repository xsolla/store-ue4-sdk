// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUtilsDataModel.h"
#include "XsollaUtilsTheme.h"

#include "XsollaUtilsSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLAUTILS_API UXsollaUtilsSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** List of customizable themes available for user interface. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Utils Settings")
	TMap<FString, FXsollaInterfaceTheme> Themes;

	/** Interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Utils Settings")
	TSubclassOf<UXsollaUtilsTheme> InterfaceTheme;
};
