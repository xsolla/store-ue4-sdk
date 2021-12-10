// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaUtilsTokenParser.generated.h"

class FJsonObject;

/**
 * JWT parsing utility.
 */
UCLASS()
class XSOLLAUTILS_API UXsollaUtilsTokenParser : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	/** Parses a JWT token and gets its payload as a JSON object. */
	static bool ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject);

	/** Extract Param from token
	 * Extracts the string parameter with specified name from Token string.
	 *
	 * @param Token User authorization token.
	 * @param ParamName Parameter name.
	 * @param ParamValue Output parameter value of extraction in case of success.
	 *
	 * @return true on success, false on any failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Utils")
	static UPARAM(DisplayName = "IsSuccessfull") bool GetStringTokenParam(const FString& Token, const FString& ParamName, FString& ParamValue);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Utils")
	static UPARAM(DisplayName = "IsSuccessfull") bool GetBoolTokenParam(const FString& Token, const FString& ParamName, bool& ParamValue);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Utils")
	static UPARAM(DisplayName = "IsSuccessfull") bool GetInt64TokenParam(const FString& Token, const FString& ParamName, int64& ParamValue);
};
