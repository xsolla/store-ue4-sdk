// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaStoreSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLASTORE_API UXsollaStoreSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Store ID from Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	FString ProjectId;

	/** Set to true to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	bool bSandbox;

	/** Is sandbox mode can be used in Shipping build? Use carefully! */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	bool bEnableSandboxInShipping;

	/** Custom class to handle payment console */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/** If yes, external (system) browser will be launched to process payments */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	bool bUsePlatformBrowser;

	/** Demo Store ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Demo")
	FString DemoProjectId;
};
