// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaLoginLibrary.generated.h"

class UXsollaLoginSubsystem;
class UXsollaLoginSettings;
class UTexture2D;

UCLASS()
class XSOLLALOGIN_API UXsollaLoginLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Gives direct access to login SDK settings. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Login")
	static UXsollaLoginSettings* GetLoginSettings();

	/** Validates the email string format. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static bool IsEmailValid(const FString& EMail);

	/** Gets the string command line parameter value. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetStringCommandLineParam(const FString& ParamName);

	/** Gets the platform specific authentication session ticket to verify user identity. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetSessionTicket();

	/** Opens specified URL in the platfrom browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Library")
	static void LaunchPlatfromBrowser(const FString& URL);

	/** Converts the 2D texture to its PNG-encoded byte representation. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Library")
	static TArray<uint8> ConvertTextureToByteArray(UTexture2D* Texture);

	/** Gets thd specified parameter from a given URL. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetUrlParameter(const FString& URL, const FString& Parameter);
};
