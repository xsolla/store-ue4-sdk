// Copyright 2019 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaPayStationSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLAPAYSTATION_API UXsollaPayStationSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** URL used to fetch token for payment console */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla PayStation Settings")
	FString TokenRequestURL;

	/** Set to true to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla PayStation Settings")
	bool bSandbox;

	/** Custom class to handle payment console */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla PayStation Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;
};