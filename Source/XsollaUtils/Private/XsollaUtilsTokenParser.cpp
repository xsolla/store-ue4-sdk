// Copyright 2024 Xsolla Inc. All Rights Reserved.

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

	const FStringView TokenPart{TokenParts[1]};
	const uint32 ExpectedLength = FBase64::GetDecodedDataSize(TokenPart.GetData(), TokenPart.Len());
	TArray<uint8> Dest;
	Dest.AddZeroed(ExpectedLength + 1);

	// JWT uses Base64URL encoding (RFC 4648) - must use UrlSafe mode for payloads with Unicode (e.g. nicknames)
	if (!FBase64::Decode(TokenPart.GetData(), TokenPart.Len(), Dest.GetData(), EBase64Mode::UrlSafe))
	{
		return false;
	}

	// Payload is UTF-8 encoded JSON
	const FString PayloadStr = UTF8_TO_TCHAR(Dest.GetData());

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
