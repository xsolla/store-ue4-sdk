// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUtilsDataModel.h"

#include "XsollaStoreSettings.generated.h"

/** User interface size for the payment interface. */
UENUM(BlueprintType)
enum class EXsollaPaymentUiSize : uint8
{
	small UMETA(DisplayName = "Small"),
	medium UMETA(DisplayName = "Medium"),
	large UMETA(DisplayName = "Large")
};

/** User interface theme for the payment interface. */
UENUM(BlueprintType)
enum class EXsollaPaymentUiTheme : uint8
{
	default_light UMETA(DisplayName = "Default"),
	default_dark UMETA(DisplayName = "Default Dark"),
	dark UMETA(DisplayName = "Dark"),
	ps4_default_light UMETA(DisplayName = "PS4 Default Light"),
	ps4_default_dark UMETA(DisplayName = "PS4 Default Dark")
};

/** Device type used to display the payment interface. */
UENUM(BlueprintType)
enum class EXsollaPaymentUiVersion : uint8
{
	not_specified UMETA(DisplayName = "Not specified"),
	desktop UMETA(DisplayName = "Desktop"),
	mobile UMETA(DisplayName = "Mobile")
};

/** Payment status that triggers user redirect to the return URL. */
UENUM(BlueprintType)
enum class EXsollaPaymentRedirectCondition : uint8
{
	none UMETA(DisplayName = "None"),
	successful UMETA(DisplayName = "Successful"),
	successful_or_canceled UMETA(DisplayName = "Successful or Cancelled"),
	any UMETA(DisplayName = "Any")
};

/** Payment status that triggers the display of a button clicking which redirects the user to the return URL. */
UENUM(BlueprintType)
enum class EXsollaPaymentRedirectStatusManual : uint8
{
	none UMETA(DisplayName = "None"),
	vc UMETA(DisplayName = "Purchase using virtual currency"),
	successful UMETA(DisplayName = "Successful"),
	successful_or_canceled UMETA(DisplayName = "Successful or Cancelled"),
	any UMETA(DisplayName = "Any")
};

UCLASS(config = Engine, defaultconfig)
class XSOLLASTORE_API UXsollaStoreSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	FString ProjectID;

	/** Custom class to handle the payment console. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/** Enable to process a payment with an external (system) browser. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	bool UsePlatformBrowser;

	/** If enabled, Store SDK will imitate platform-specific requests so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Use Cross-Platform Account Linking", Category = "Xsolla Store Settings")
	bool UseCrossPlatformAccountLinking;

	/** Target platform for cross-platform account linking. If you use Xsolla Login, make sure that you choose the same platform in the Login settings. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	EXsollaPublishingPlatform Platform;

	/** Enable to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	bool EnableSandbox;

	/** Enable if the sandbox mode can be used in a shipping build. Use carefully! */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	bool EnableSandboxInShippingBuild;

	/** Enable to process payment tasks via Steam. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Settings")
	bool BuildForSteam;

	/** Payment user interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Payments UI")
	EXsollaPaymentUiTheme PaymentInterfaceTheme;

	/** Payment user interface size (desktop only).*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Payments UI")
	EXsollaPaymentUiSize PaymentInterfaceSize;

	/** Device type used to present payment interface. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Payments UI")
	EXsollaPaymentUiVersion PaymentInterfaceVersion;

	/** Enable to override default redirect policy for payments. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Redirect Policy")
	bool OverrideRedirectPolicy;

	/** Page to redirect user to after the payment. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	FString ReturnUrl;

	/** Payment status that triggers user redirect to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	EXsollaPaymentRedirectCondition RedirectCondition;

	/** Delay after which the user will be automatically redirected to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	int32 RedirectDelay;

	/** Payment status triggering the display of a button clicking which redirects the user to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	EXsollaPaymentRedirectStatusManual RedirectStatusManual;

	/** Redirect button caption. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	FString RedirectButtonCaption;

	/** Demo Project ID. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Demo")
	FString DemoProjectID;

	/** Enable deep linking for Android applications. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Android")
	bool UseDeepLinking;

	/** Redirect URL that allows external applications to enter current project game activity. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Store Android", meta = (EditCondition = "UseDeepLinking"))
	FString RedirectURL;

	UFUNCTION(BlueprintCallable, Category = "Xsolla Store Settings")
	void SetupDefaultDemoSettings();
};
