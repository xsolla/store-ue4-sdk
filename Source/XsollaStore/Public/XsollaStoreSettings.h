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
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	FString ProjectID;

	/** Enable to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	bool EnableSandbox;

	/** Enable if the sandbox mode can be used in a shipping build. Use carefully! */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	bool EnableSandboxInShippingBuild;

	/** Payment user interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	EXsollaPaymentUiTheme PaymentInterfaceTheme;

	/** Custom class to handle payment console. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/** Enable to process a payment with an external (system) browser. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	bool UsePlatformBrowser;

	/** Enable to process tasks such as authentication and payment via Steam. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings")
	bool BuildForSteam;

	/** If enabled, Store SDK will imitate platform-specific requests so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, DisplayName = "Use Cross-Platform Account Linking", Category = "Xsolla Store Settings")
	bool UseCrossPlatformAccountLinking;

	/** Target platform for cross-platform account linking. If using Xsolla Login, make sure that in the Login settings the same platform is chosen. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Settings", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	EXsollaPublishingPlatform Platform;

	/** Demo Project ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Store Demo")
	FString DemoProjectID;
};
