// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginDefines.h"

#include "Blueprint/UserWidget.h"

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

/** Target platform name */
UENUM(BlueprintType)
enum class EXsollaTargetPlatform : uint8
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
class XSOLLALOGIN_API UXsollaLoginSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString ProjectID;

	/** Login ID in the UUID format from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString LoginID;

	/**
	 * URL to redirect the user to after registration/authentication/password reset.
	 * Must be identical to a Callback URL specified in Publisher Account in Login settings.
	 * Required if there are several Callback URLs.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString CallbackURL;

	/** API methods will be calling different URLs depending on the selected storage method. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	EUserDataStorage UserDataStorage;

	/** Custom class to handle authentication via social network. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/** If enabled, Login SDK will deactivate the existing user JWT values and activate the one generated during last successful authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, DisplayName = "Invalidate Existing Sessions", Category = "Xsolla Login Settings")
	bool InvalidateExistingSessions;

	/** If enabled, Login SDK will use OAuth 2.0 protocol in order to authorize user. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, DisplayName = "Use OAuth2", Category = "Xsolla Login Settings")
	bool UseOAuth2;

	/** Your application ID. You will get it after sending request to enable the OAuth 2.0 protocol. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "UseOAuth2"))
	FString ClientID;

	/** If enabled, Login SDK will imitate platform-specific authentication so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, DisplayName = "Use Cross-Platform Account Linking", Category = "Xsolla Login Settings")
	bool UseCrossPlatformAccountLinking;

	/**
	 * URL used to link the user platform account to the main account with a generated code.
	 * The main account is the Xsolla Login project which other Xsolla Login projects (platform accounts) are linked to.
	 * Main and platform accounts are created in Publisher Account.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	FString AccountLinkingURL;

	/** URL used for a target platform user account authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	FString PlatformAuthenticationURL;

	/** Target platform for cross-platform account linking. If using Xsolla Store, make sure that in the Store settings the same platform is chosen. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	EXsollaTargetPlatform Platform;

	/** Unique identifier of a target platform user account. You can enter any alphanumeric combination. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings",
		meta = (EditCondition = "UseCrossPlatformAccountLinking && Platform != EXsollaTargetPlatform::Xsolla"))
	FString PlatformAccountID;

	/** Flag indicating whether Xsolla cached credentials should be encrypted and decrypted using the XsollaSaveEncryptionKey secondary encryption key */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	bool EncryptCachedCredentials;

	/** AES-256 encryption key used for cached credentials encryption */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "EncryptCachedCredentials"))
	FString XsollaSaveEncryptionKey;

	/** Demo Project ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Demo")
	FString DemoProjectID;

	/** Demo Login ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Demo")
	FString DemoLoginID;

	/** Request user nickname after successful authorization in case one is missing */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Demo")
	bool RequestNickname;
};
