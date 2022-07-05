// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "XsollaSettingsTypes.h"
#include "XsollaProjectSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLASETTINGS_API UXsollaProjectSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString ProjectID;

	/** Login ID in the UUID format from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString LoginID;

	/** Your application ID. You will get it after sending request to enable the OAuth 2.0 protocol. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	FString ClientID;

	/** Redirect uri */
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

	/** Enable to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Purchases")
	bool EnableSandbox;
	
	/** If enabled, SDK will imitate platform-specific authentication so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Use Cross-Platform Account Linking", Category = "Demo")
	bool UseCrossPlatformAccountLinking;

	/**
	 * URL used to link the user platform account to the main account with a generated code.
	 * The main account is the Xsolla Login project which other Xsolla Login projects (platform accounts) are linked to.
	 * Main and platform accounts are created in Publisher Account.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	FString AccountLinkingURL;

	/** URL used for a target platform user account authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	FString PlatformAuthenticationURL;

	/** Target platform for cross-platform account linking. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	EXsollaPublishingPlatform Platform;

	/** Unique identifier of a target platform user account. You can enter any alphanumeric combination. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Demo",
		meta = (EditCondition = "UseCrossPlatformAccountLinking && Platform != EXsollaPublishingPlatform::xsolla"))
	FString PlatformAccountID;

	/** Web Store URL to be opened in order to purchase virtual items. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FString WebStoreURL;

	/** If enabled, user authentication will be handled via native Android applications if possible. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android")
	bool bAllowNativeAuth;
	
	/** Facebook app identifier (can be obtained on Facebook developer page). Used for native user authentication via Facebook Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android", meta = (EditCondition = "bAllowNativeAuth"))
	FString FacebookAppId;

	/** Google app identifier (can be obtained on Google developer page). Used for native user authentication via Google Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android", meta = (EditCondition = "bAllowNativeAuth"))
	FString GoogleAppId;

	/** WeChat app identifier (can be obtained on WeChat developer page). Used for native user authentication via WeChat Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android", meta = (EditCondition = "bAllowNativeAuth"))
	FString WeChatAppId;

	/** QQ app identifier (can be obtained on QQ developer page). Used for native user authentication via QQ Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Android", meta = (EditCondition = "bAllowNativeAuth"))
	FString QQAppId;
	
	/** Payment user interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Paystation UI")
	EXsollaPaymentUiTheme PaymentInterfaceTheme;

	/** Payment user interface size (desktop only).*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Paystation UI")
	EXsollaPaymentUiSize PaymentInterfaceSize;

	/** Device type used to present payment interface. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Paystation UI")
	EXsollaPaymentUiVersion PaymentInterfaceVersion;

	/** Enable to override default redirect policy for payments. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy")
	bool UseSettingsFromPublisherAccount;

	/** Page to redirect user to after the payment. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	FString ReturnUrl;

	/** Payment status that triggers user redirect to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	EXsollaPaymentRedirectCondition RedirectCondition;

	/** Delay after which the user will be automatically redirected to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	int32 RedirectDelay;

	/** Payment status triggering the display of a button clicking which redirects the user to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	EXsollaPaymentRedirectStatusManual RedirectStatusManual;

	/** Redirect button caption. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Redirect Policy", meta = (EditCondition = "!UseSettingsFromPublisherAccount"))
	FString RedirectButtonCaption;
};
