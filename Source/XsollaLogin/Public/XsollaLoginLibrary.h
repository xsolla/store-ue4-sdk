// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaLoginLibrary.generated.h"

class UXsollaLoginSubsystem;
class UTexture2D;

UCLASS()
class XSOLLALOGIN_API UXsollaLoginLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Validates the email string format. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static bool IsEmailValid(const FString& EMail);

	/** Returns the string command line parameter value. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetStringCommandLineParam(const FString& ParamName);

	/** Returns the specific platform authentication session ticket to verify user's identity. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetSessionTicket();

	/** Opens the specified URL in a platfrom browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Library")
	static void LaunchPlatfromBrowser(const FString& URL);

	/** Converts a 2D texture into its PNG-encoded byte representation. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Library")
	static TArray<uint8> ConvertTextureToByteArray(const UTexture2D* Texture);

	/** Returns the specified parameter from a given URL. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetUrlParameter(const FString& URL, const FString& Parameter);

	/** Returns the device name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetDeviceName();

	/** Returns the device ID. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetDeviceId();

	/** Returns the app ID. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetAppId();

	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static bool IsSteamBuildValid(FString& OutError);
};
