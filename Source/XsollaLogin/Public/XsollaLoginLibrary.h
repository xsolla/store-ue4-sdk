// Copyright 2021 Xsolla Inc. All Rights Reserved.

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

	/** Returnsthe string command line parameter value. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetStringCommandLineParam(const FString& ParamName);

	/** Returnsthe specific platform authentication session ticket to verify user's identity. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetSessionTicket();

	/** Opens the specified URL in a platfrom browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Library")
	static void LaunchPlatfromBrowser(const FString& URL);

	/** Converts a 2D texture into its PNG-encoded byte representation. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Library")
	static TArray<uint8> ConvertTextureToByteArray(const UTexture2D* Texture);

	/** Returnsthe specified parameter from a given URL. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetUrlParameter(const FString& URL, const FString& Parameter);

	/** Returnsthe device name. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetDeviceName();

	/** Returnsthe device ID. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetDeviceId();

	/** Returnsthe app ID. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Library")
	static FString GetAppId();
};
