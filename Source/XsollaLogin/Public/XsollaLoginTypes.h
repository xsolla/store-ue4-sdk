// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginTypes.generated.h"

UENUM(BlueprintType)
enum class EXsollaAttributePermission : uint8
{
	Private,
	Public
};

USTRUCT(BlueprintType)
struct FXsollaAuthToken
{
	GENERATED_BODY()

	/** A JWT signed by the secret key is generated for each successfully authenticated user. */
	UPROPERTY(BlueprintReadOnly, Category = "Auth Token")
	FString JWT;

	/** Token verification status. */
	UPROPERTY(BlueprintReadOnly, Category = "Auth Token")
	bool bIsVerified;

	FXsollaAuthToken()
		: bIsVerified(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaLoginData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	FXsollaAuthToken AuthToken;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	FString Username;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	FString Password;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	bool bRememberMe;

	FXsollaLoginData()
		: bRememberMe(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaUserAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "User Attribute")
	FString key;

	UPROPERTY(BlueprintReadOnly, Category = "User Attribute")
	FString permission;

	UPROPERTY(BlueprintReadOnly, Category = "User Attribute")
	FString value;
};
