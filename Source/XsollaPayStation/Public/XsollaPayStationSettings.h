// Copyright 2019 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaPayStationSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLAPAYSTATION_API UXsollaPayStationSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** URL used to fetch a token for a payment console */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla PayStation Settings")
	FString TokenRequestURL;

	/** Enable to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla PayStation Settings")
	bool EnableSandbox;

	/** Enable if the sandbox mode can be used in a shipping build. Use carefully! */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla PayStation Settings")
	bool EnableSandboxInShippingBuild;

	/** Custom class to handle payment console. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla PayStation Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;
};