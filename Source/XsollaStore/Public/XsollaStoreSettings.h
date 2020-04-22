// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaStoreSettings.generated.h"

/** User interface theme for payment interface*/
UENUM(BlueprintType)
enum class EXsollaPaymentUiTheme : uint8
{
	Default,
	DefaultDark,
	Dark
};

/** Target platform name */
UENUM(BlueprintType)
enum class EXsollaPublishingPlatform : uint8
{
	NotDefined,
	PlaystationNetwork,
	XboxLive,
	Xsolla,
	PcStandalone,
	NintendoShop,
	GooglePlay,
	AppStoreIos,
	AndroidStandalone,
	IosStandalone,
	AndroidOther,
	IosOther,
	PcOther
};

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

	/** If yes, certain tasks like authentication and payments will be processed via Steam */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	bool bBuildForSteam;

	/** Payment user interface theme */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	EXsollaPaymentUiTheme PaymentInterfaceTheme;

	/** Target platform for project publishing */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	EXsollaPublishingPlatform PublishingPlatform;

	/** Demo Store ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Demo")
	FString DemoProjectId;
};
