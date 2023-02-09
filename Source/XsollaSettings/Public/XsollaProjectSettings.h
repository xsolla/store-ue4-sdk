// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "XsollaSettingsTypes.h"
#include "XsollaProjectSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLASETTINGS_API UXsollaProjectSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/**
	 * Project ID from Publisher Account.
	 * It can be found in Publisher Account next to the name of your project.
	 * Required.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString ProjectID;

	/**
	 * Login ID in the UUID format from Publisher Account.
	 * To get it, open Publisher Account, go to the **Login > Dashboard** section, and click **Copy ID** beside the name of the Login project.
	 * If you don't use Xsolla Login, leave this field blank.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString LoginID;

	/**
	 * Your application ID. You will get it after sending request to enable the OAuth 2.0 protocol.
	 * Can be found in Publisher Account.
	 * To get the ID, set up an OAuth client in Publisher Account in the "Login -> your Login project -> Secure -> OAuth 2.0" section.
	 * If you don't use Xsolla Login, leave this field blank.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString ClientID;

	/**
	 * URI to redirect the user to after signing up, logging in, or password reset.
	 * Must be identical to the OAuth 2.0 redirect URI specified in Publisher Account in Login -> Security -> OAuth 2.0 settings.
	 * Required if there are several callback URIs.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString RedirectURI;

	/** Flag indicating whether Xsolla cached credentials should be encrypted and decrypted using the XsollaSaveEncryptionKey secondary encryption key. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Security")
	bool EncryptCachedCredentials;

	/** AES-256 encryption key used for cached credentials encryption. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Security", meta = (EditCondition = "EncryptCachedCredentials"))
	FString XsollaSaveEncryptionKey;

	/** Flag indicating whether the authentication link should be sent together with the confirmation code for passwordless login. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Passwordless authentication")
	bool SendPasswordlessAuthURL;

	/** Authentication link used for passwordless login. It won't be sent together with the confirmation code for passwordless login if empty. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Passwordless authentication", meta = (EditCondition = "SendPasswordlessAuthURL"))
	FString PasswordlessAuthURL;

	/** Enable to process a payment with an external (system) browser. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Purchases")
	bool UsePlatformBrowser;

	/**
	 * Enable to test the payment process.
	 * sandbox-secure.xsolla.com will be used instead of secure.xsolla.com.
	 * If you already accepted a real payment, test payments are available only to users
	 * who are specified in Publisher Account in the "Company settings -> Users" section.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Purchases")
	bool EnableSandbox;

	/** Web Store URL to be opened in order to purchase virtual items. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FString WebStoreURL;

	/** Facebook app identifier (can be obtained on Facebook developer page). Used for native user authentication via Facebook Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android")
	FString FacebookAppId;

	/** Google app identifier (can be obtained on Google developer page). Used for native user authentication via Google Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android")
	FString GoogleAppId;

	/** WeChat app identifier (can be obtained on WeChat developer page). Used for native user authentication via WeChat Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android")
	FString WeChatAppId;

	/** QQ app identifier (can be obtained on QQ developer page). Used for native user authentication via QQ Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android")
	FString QQAppId;

	/** Payment user interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Paystation UI")
	EXsollaPaymentUiTheme PaymentInterfaceTheme;

	/**
	 * Payment user interface size (desktop only).
	 * Small: 620 x 630 px
	 * Medium (recomended): 740 x 760 px
	 * Large: 820 x 840 px
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Paystation UI")
	EXsollaPaymentUiSize PaymentInterfaceSize;

	/**
	 * Device type used to present payment interface.
	 * The Pay Station UI depends on the device type.
	 * If Not specified, the app automatically uses Pay Station UI supported by the device.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Paystation UI")
	EXsollaPaymentUiVersion PaymentInterfaceVersion;

	/**
	 * Use settings from Publisher Account.
	 * If the option is enabled, SDK uses redirect settings
	 * specified in Publisher Account in the "Pay Station -> Settings -> Redirect policy" section.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy")
	bool UseSettingsFromPublisherAccount;

	/** Page to redirect user to after the payment. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	FString ReturnUrl;

	/**
	 * Redirect Conditions.
	 * Payment status when user is automatically redirected to the return URL.
	 * "None" - do not redirect.
	 * "Successful payment" - redirect when a payment is successful.
	 * "Successful or canceled payment" - redirect when a payment is successful or canceled.
	 * "Any payment" - redirect for any payment status.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	EXsollaPaymentRedirectCondition RedirectCondition;

	/**
	 * Delay after which the user will be automatically redirected to the return URL.
	 * In seconds.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	int32 RedirectDelay;

	/**
	 * Status for Manual Redirection.
	 * Payment status when the redirect button appears.
	 * "None" - do not redirect.
	 * "Purchase using virtual currency" - redirect when purchase is made using virtual currency.
	 * "Successful payment" - redirect when a payment is successful.
	 * "Successful or canceled payment" - redirect when a payment is successful or canceled.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	EXsollaPaymentRedirectStatusManual RedirectStatusManual;

	/**
	 * Redirect button caption.
	 * Caption of the button that will redirect the user to the return URL.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	FString RedirectButtonCaption;
};
