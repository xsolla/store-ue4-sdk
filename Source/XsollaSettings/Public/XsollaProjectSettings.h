// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "XsollaProjectSettings.generated.h"

/** You can store user data at Xsolla's side, which is the default option, or in your own storage. */
UENUM(BlueprintType)
enum class EUserDataStorage : uint8
{
	/** User data is stored at Xsolla's side */
	Xsolla UMETA(DisplayName = "Xsolla storage"),

	/** If the user data is stored on your side, proxy requests are used. */
	Custom UMETA(DisplayName = "Custom storage"),
};

/** Target platform name */
UENUM(BlueprintType)
enum class EXsollaPublishingPlatform : uint8
{
	playstation_network UMETA(DisplayName = "PlaystationNetwork"),
	xbox_live UMETA(DisplayName = "XboxLive"),
	xsolla UMETA(DisplayName = "Xsolla"),
	pc_standalone UMETA(DisplayName = "PcStandalone"),
	nintendo_shop UMETA(DisplayName = "NintendoShop"),
	google_play UMETA(DisplayName = "GooglePlay"),
	app_store_ios UMETA(DisplayName = "AppStoreIos"),
	android_standalone UMETA(DisplayName = "AndroidStandalone"),
	ios_standalone UMETA(DisplayName = "IosStandalone"),
	android_other UMETA(DisplayName = "AndroidOther"),
	ios_other UMETA(DisplayName = "IosOther"),
	pc_other UMETA(DisplayName = "PcOther")
};

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
class XSOLLASETTINGS_API UXsollaProjectSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString ProjectID;

	/** Login ID in the UUID format from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString LoginID;

	/**
	 * URL to redirect the user to after registration/authentication/password reset.
	 * Must be identical to a Callback URL specified in Publisher Account in Login settings.
	 * Required if there are several Callback URLs.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken&&!UseOAuth2", EditConditionHides))
	FString CallbackURL;

	/**
	 * Redirect uri
	 */
	//UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken&&UseOAuth2", EditConditionHides))
	//FString RedirectURI;

	/** API methods will be calling different URLs depending on the selected storage method. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	EUserDataStorage UserDataStorage;

	/** If enabled, SDK will deactivate the existing user JWT values and activate the one generated during last successful authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Invalidate Existing Sessions", Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool InvalidateExistingSessions;

	/** If enabled, SDK will use OAuth 2.0 protocol in order to authorize user. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Use OAuth2", Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool UseOAuth2;

	/** Your application ID. You will get it after sending request to enable the OAuth 2.0 protocol. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "UseOAuth2 && !bCustomAuthViaAccessToken"))
	FString ClientID;

	/** Flag indicating whether Xsolla cached credentials should be encrypted and decrypted using the XsollaSaveEncryptionKey secondary encryption key. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool EncryptCachedCredentials;

	/** AES-256 encryption key used for cached credentials encryption. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "EncryptCachedCredentials && !bCustomAuthViaAccessToken"))
	FString XsollaSaveEncryptionKey;

	/** Flag indicating whether the authentication link should be sent together with the confirmation code for passwordless login. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool SendPasswordlessAuthURL;

	/** Authentication link used for passwordless login. It won't be sent together with the confirmation code for passwordless login if empty. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "SendPasswordlessAuthURL && !bCustomAuthViaAccessToken"))
	FString PasswordlessAuthURL;

	/** Enable to process a payment with an external (system) browser. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings")
	bool UsePlatformBrowser;

	/** Enable to test the payment process: sandbox-secure.xsolla.com will be used instead of secure.xsolla.com. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings")
	bool EnableSandbox;
	
	/** If enabled, SDK will imitate platform-specific authentication so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Use Cross-Platform Account Linking", Category = "Xsolla Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool UseCrossPlatformAccountLinking;

	/**
	 * URL used to link the user platform account to the main account with a generated code.
	 * The main account is the Xsolla Login project which other Xsolla Login projects (platform accounts) are linked to.
	 * Main and platform accounts are created in Publisher Account.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking && !bCustomAuthViaAccessToken"))
	FString AccountLinkingURL;

	/** URL used for a target platform user account authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking && !bCustomAuthViaAccessToken"))
	FString PlatformAuthenticationURL;

	/** Target platform for cross-platform account linking. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking && !bCustomAuthViaAccessToken"))
	EXsollaPublishingPlatform Platform;

	/** Unique identifier of a target platform user account. You can enter any alphanumeric combination. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo",
		meta = (EditCondition = "UseCrossPlatformAccountLinking && Platform != EXsollaPublishingPlatform::xsolla && !bCustomAuthViaAccessToken"))
	FString PlatformAccountID;

	/** Web Store URL to be opened in order to purchase virtual items. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo")
	FString WebStoreURL;
	
	/** If enabled, Inventory SDK will user auth via access token */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Custom Auth")
	bool bCustomAuthViaAccessToken;

	/** URL for login via access token */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Custom Auth", meta = (EditCondition = "bCustomAuthViaAccessToken"))
	FString CustomAuthServerURL;

	/** If enabled, user authentication will be handled via native Android applications if possible. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool bAllowNativeAuth;

	/** Facebook app identifier (can be obtained on Facebook developer page). Used for native user authentication via Facebook Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android", meta = (EditCondition = "bAllowNativeAuth && !bCustomAuthViaAccessToken"))
	FString FacebookAppId;

	/** Google app identifier (can be obtained on Google developer page). Used for native user authentication via Google Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android", meta = (EditCondition = "bAllowNativeAuth && !bCustomAuthViaAccessToken"))
	FString GoogleAppId;

	/** WeChat app identifier (can be obtained on WeChat developer page). Used for native user authentication via WeChat Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android", meta = (EditCondition = "bAllowNativeAuth && !bCustomAuthViaAccessToken"))
	FString WeChatAppId;

	/** QQ app identifier (can be obtained on QQ developer page). Used for native user authentication via QQ Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android", meta = (EditCondition = "bAllowNativeAuth && !bCustomAuthViaAccessToken"))
	FString QQAppId;

	/** Enable deep linking for Android applications. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android")
	bool UseDeepLinking;

	/** Redirect URL that allows external applications to enter current project game activity. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Android", meta = (EditCondition = "UseDeepLinking"))
	FString RedirectURL;
	
	/** Payment user interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Payments UI")
	EXsollaPaymentUiTheme PaymentInterfaceTheme;

	/** Payment user interface size (desktop only).*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Payments UI")
	EXsollaPaymentUiSize PaymentInterfaceSize;

	/** Device type used to present payment interface. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Payments UI")
	EXsollaPaymentUiVersion PaymentInterfaceVersion;

	/** Enable to override default redirect policy for payments. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Redirect Policy")
	bool OverrideRedirectPolicy;

	/** Page to redirect user to after the payment. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	FString ReturnUrl;

	/** Payment status that triggers user redirect to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	EXsollaPaymentRedirectCondition RedirectCondition;

	/** Delay after which the user will be automatically redirected to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	int32 RedirectDelay;

	/** Payment status triggering the display of a button clicking which redirects the user to the return URL. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	EXsollaPaymentRedirectStatusManual RedirectStatusManual;

	/** Redirect button caption. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Redirect Policy", meta = (EditCondition = "OverrideRedirectPolicy"))
	FString RedirectButtonCaption;

	/** Enable to process payment tasks via Steam. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings")
	bool BuildForSteam;

	/** Custom class to handle the payment console. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings")
	TSubclassOf<UUserWidget> OverrideStoreBrowserWidgetClass;

	/** Custom class to handle authentication via social network. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	TSubclassOf<UUserWidget> OverrideLoginBrowserWidgetClass;
	
	/** If enabled, Login SDK will use Steam as default authorization mechanism */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool bForceSteamAuthorization;

	/** Demo Project ID. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString DemoProjectID;

	/** Demo Login ID. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString DemoLoginID;

	/** Demo application ID. You will get it after sending request to enable the OAuth 2.0 protocol. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Demo", meta = (EditCondition = "UseOAuth2 && !bCustomAuthViaAccessToken"))
	FString DemoClientID;
};
