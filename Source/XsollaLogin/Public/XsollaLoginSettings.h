// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

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

UCLASS(config = Engine, defaultconfig)
class XSOLLALOGIN_API UXsollaLoginSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
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

	/**
	 * A JWT signed by the secret key is generated for each successfully authenticated user.
	 * To make sure that the JWT has not expired and belongs to the user in your project, you need to validate its value.
	 * Check *Extras/TokenVerificator* folder for example of verification server app.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Login Settings", meta = (DisplayName = "JWT Validation URL"))
	FString VerifyTokenURL;
};
