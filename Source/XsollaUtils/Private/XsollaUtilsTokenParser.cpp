// Copyright 2021 Xsolla Inc. All Rights Reserved.


#include "XsollaUtilsTokenParser.h"

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

bool UXsollaUtilsTokenParser::GetStringTokenParam(const FString& Token, const FString& ParamName, FString& ParamValue, FString& OutError)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		OutError = TEXT("Can't parse token payload");
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: %s"), *VA_FUNC_LINE, *OutError);
		return false;
	}

	if (!PayloadJsonObject->TryGetStringField(ParamName, ParamValue))
	{
		OutError = FString::Printf(TEXT("Can't find %s in token payload"), *ParamName);
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: %s"), *VA_FUNC_LINE, *OutError);
		return false;
	}
	return true;
}

bool UXsollaUtilsTokenParser::GetStringTokenParam(const FString& Token, const FString& ParamName, FString& ParamValue)
{
	FString OutError;
	return GetStringTokenParam(Token, ParamName, ParamValue, OutError);
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
		return false;
	}

	return true;
}
