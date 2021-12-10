// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsTokenParser.h"
#include "XsollaUtilsDefines.h"

#include "Dom/JsonObject.h"
#include "Misc/Base64.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UXsollaUtilsTokenParser::UXsollaUtilsTokenParser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UXsollaUtilsTokenParser::ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject)
{
	TArray<FString> TokenParts;

	Token.ParseIntoArray(TokenParts, TEXT("."));
	if (TokenParts.Num() <= 1)
	{
		return false;
	}

	FString PayloadStr;
	if (!FBase64::Decode(TokenParts[1], PayloadStr))
	{
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PayloadStr);
	if (!FJsonSerializer::Deserialize(Reader, PayloadJsonObject))
	{
		return false;
	}

	return true;
}

bool UXsollaUtilsTokenParser::GetStringTokenParam(const FString& Token, const FString& ParamName, FString& ParamValue)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return false;
	}

	if (!PayloadJsonObject->TryGetStringField(ParamName, ParamValue))
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't find %s in token payload"), *VA_FUNC_LINE, *ParamName);
		return false;
	}

	return true;
}

bool UXsollaUtilsTokenParser::GetBoolTokenParam(const FString& Token, const FString& ParamName, bool& ParamValue)
{
	ParamValue = false;
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return false;
	}

	if (!PayloadJsonObject->TryGetBoolField(ParamName, ParamValue))
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't find %s in token payload"), *VA_FUNC_LINE, *ParamName);
		return false;
	}

	return true;
}

bool UXsollaUtilsTokenParser::GetInt64TokenParam(const FString& Token, const FString& ParamName, int64& ParamValue)
{
	ParamValue = 0;
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return false;
	}

	if (!PayloadJsonObject->TryGetNumberField(ParamName, ParamValue))
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't find %s in token payload"), *VA_FUNC_LINE, *ParamName);
		return false;
	}

	return true;
}
