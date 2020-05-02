// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginDefines.h"

#include "Blueprint/UserWidget.h"

#include "XsollaLoginSettings.generated.h"

/** You can store user data at Xsolla's side, which is the default option, or in your own storage. */
UENUM()
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
	/** Project ID from Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString ProjectId;

	/** Login ID from Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString LoginProjectID;

	/**
	 * URL to generate the link with additional parameters and to redirect the user to
	 * after account confirmation. Must be identical to the Callback URL
	 * specified in Publisher Account in Login settings.
	 * Required if there are several Callback URLs.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString CallbackURL;

	/** API methods will be calling different URLs depending on the selected storage method. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	EUserDataStorage UserDataStorage;

	/** Custom class to handle authentication via social networks */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/**
	 * A JWT signed by the secret key is generated for each successfully authenticated user.
	 * To make sure that the JWT has not expired and belongs to the user in your project, you need to validate its value.
	 * Check *Extras/TokenVerificator* folder for example of verification server app.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (DisplayName = "JWT Validation URL"))
	FString VerifyTokenURL;

	/**
	 * URL used to link user platform account to main account with generated code.
	 * Main account is the Xsolla Login project which other Xsolla Login projects (platform accounts)
	 * are linked to. Main and platform accounts are created in Publisher Account.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString AccountLinkingURL;

	/** Flag indicating whether Xsolla cached credentials should be encrypted and decrypted using the XsollaSaveEncryptionKey secondary encryption key */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	bool EncryptCachedCredentials;
	
	/** AES-256 encryption key to use when encrypting Xsolla cached credentials */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	FString XsollaSaveEncryptionKey;
	
	/** Flag indicating whether this build is targeted for specific platform */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings")
	bool PlatformBuild;

	/** URL used for target platform (shadow) user account authentication. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "PlatformBuild"))
	FString PlatformAuthenticationURL;

	/** Target platform. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "PlatformBuild"))
	EXsollaTargetPlatform Platform;

	/** Unique identifier of target platform user account. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (EditCondition = "PlatformBuild && Platform != EXsollaTargetPlatform::Xsolla"))
	FString PlatformUserId;

	/** Demo Project ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Demo")
	FString DemoProjectId;

	/** Demo Login ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Demo")
	FString DemoLoginProjectID;
};
