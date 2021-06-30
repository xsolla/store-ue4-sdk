// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Blueprint/UserWidget.h"
#include "XsollaUtilsDataModel.h"

#include "XsollaLoginSettings.generated.h"

/** You can store user data at Xsolla's side, which is the default option, or in your own storage. */
UENUM(BlueprintType)
enum class EUserDataStorage : uint8
{
	/** User data is stored at Xsolla's side */
	Xsolla UMETA(DisplayName = "Xsolla storage"),

	/** If the user data is stored on your side, proxy requests are used. */
	Custom UMETA(DisplayName = "Custom storage"),
};

UCLASS(config = Engine, defaultconfig)
class XSOLLALOGIN_API UXsollaLoginSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString ProjectID;

	/** Login ID in the UUID format from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString LoginID;

	/**
	 * URL to redirect the user to after registration/authentication/password reset.
	 * Must be identical to a Callback URL specified in Publisher Account in Login settings.
	 * Required if there are several Callback URLs.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString CallbackURL;

	/** API methods will be calling different URLs depending on the selected storage method. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	EUserDataStorage UserDataStorage;

	/** Custom class to handle authentication via social network. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/** If enabled, Login SDK will deactivate the existing user JWT values and activate the one generated during last successful authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Invalidate Existing Sessions", Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool InvalidateExistingSessions;

	/** If enabled, Login SDK will use OAuth 2.0 protocol in order to authorize user. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Use OAuth2", Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool UseOAuth2;

	/** Your application ID. You will get it after sending request to enable the OAuth 2.0 protocol. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "UseOAuth2 && !bCustomAuthViaAccessToken"))
	FString ClientID;

	/** Flag indicating whether Xsolla cached credentials should be encrypted and decrypted using the XsollaSaveEncryptionKey secondary encryption key */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool EncryptCachedCredentials;

	/** AES-256 encryption key used for cached credentials encryption */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Settings", meta = (EditCondition = "EncryptCachedCredentials && !bCustomAuthViaAccessToken"))
	FString XsollaSaveEncryptionKey;

	/** Demo Project ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString DemoProjectID;

	/** Demo Login ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	FString DemoLoginID;

	/** Demo application ID. You will get it after sending request to enable the OAuth 2.0 protocol. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "UseOAuth2 && !bCustomAuthViaAccessToken"))
	FString DemoClientID;

	/** If enabled, Login SDK will imitate platform-specific authentication so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, DisplayName = "Use Cross-Platform Account Linking", Category = "Xsolla Login Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool UseCrossPlatformAccountLinking;

	/**
	 * URL used to link the user platform account to the main account with a generated code.
	 * The main account is the Xsolla Login project which other Xsolla Login projects (platform accounts) are linked to.
	 * Main and platform accounts are created in Publisher Account.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking && !bCustomAuthViaAccessToken"))
	FString AccountLinkingURL;

	/** URL used for a target platform user account authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking && !bCustomAuthViaAccessToken"))
	FString PlatformAuthenticationURL;

	/** Target platform for cross-platform account linking. If using Xsolla Store, make sure that in the Store settings the same platform is chosen. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "UseCrossPlatformAccountLinking && !bCustomAuthViaAccessToken"))
	EXsollaPublishingPlatform Platform;

	/** Unique identifier of a target platform user account. You can enter any alphanumeric combination. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo",
		meta = (EditCondition = "UseCrossPlatformAccountLinking && Platform != EXsollaPublishingPlatform::xsolla && !bCustomAuthViaAccessToken"))
	FString PlatformAccountID;

	/** Request user nickname after successful authorization in case one is missing */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool RequestNickname;

	/** If enabled, Login SDK will use Steam as default authorization mechanism */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Demo", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool bForceSteamAuthorization;

	UFUNCTION(BlueprintCallable, Category = "Xsolla Login Demo")
	void SetupDefaultDemoSettings();

	/** If enabled, Inventory SDK will user auth via access token */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Custom Auth")
	bool bCustomAuthViaAccessToken;

	/** URL for login via access token */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Custom Auth", meta = (EditCondition = "bCustomAuthViaAccessToken"))
	FString CustomAuthServerURL;

	/** If enabled, user authentication will be handled via native Android applications if possible. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Android", meta = (EditCondition = "!bCustomAuthViaAccessToken"))
	bool bAllowNativeAuth;

	/** Facebook app identifier (can be obtained on Facebook developer page). Used for native user authentication via Facebook Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Android", meta = (EditCondition = "bAllowNativeAuth && !bCustomAuthViaAccessToken"))
	FString FacebookAppId;

	/** Google app identifier (can be obtained on Google developer page). Used for native user authentication via Google Android application. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Login Android", meta = (EditCondition = "bAllowNativeAuth && !bCustomAuthViaAccessToken"))
	FString GoogleAppId;
};
