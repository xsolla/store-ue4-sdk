// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "JsonObjectConverter.h"

class FJsonObject;

enum class EXsollaHttpRequestVerb : uint8
{
	VERB_GET,
	VERB_POST,
	VERB_PUT,
	VERB_DELETE,
	VERB_PATCH
};

struct XSOLLAUTILS_API XsollaHttpRequestError
{
	FString code;
	FString description;
	FString errorMessage;
	int32 statusCode;
	int32 errorCode;

	XsollaHttpRequestError()
	{
	}

	XsollaHttpRequestError(const FString& errorMsg)
		: description(errorMsg)
		, errorMessage(errorMsg)
	{
	}
};

class XSOLLAUTILS_API XsollaUtilsHttpRequestHelper
{
public:
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url,
		const EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET,
		const FString& AuthToken = FString(),
		const FString& Content = FString(),
		const FString& SdkModuleName = FString(),
		const FString& SdkModuleVersion = FString());

	static bool ParseResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, XsollaHttpRequestError& OutError);
	static bool ParseResponseAsJson(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, TSharedPtr<FJsonObject>& OutResponse, XsollaHttpRequestError& OutError);
	static bool ParseResponseAsStruct(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, const UStruct* OutResponseDefinition, void* OutResponse, XsollaHttpRequestError& OutError);
	
	template <typename OutStructType>
	static bool ParseResponseAsArray(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, TArray<OutStructType>* OutResponse, XsollaHttpRequestError& OutError);
	
	static bool ParseError(TSharedPtr<FJsonObject> JsonObject, XsollaHttpRequestError& OutError);

private:

	static const FString NoResponseErrorMsg;
	static const FString UnknownErrorMsg;
	static const FString DeserializationErrorMsg;
	static const FString ConversionErrorMsg;
};

template <typename OutStructType>
bool XsollaUtilsHttpRequestHelper::ParseResponseAsArray(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, TArray<OutStructType>* OutResponse, XsollaHttpRequestError& OutError)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString ResponseStr = HttpResponse->GetContentAsString();

		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseStr, OutResponse, 0, 0))
			{
				return true;
			}

			OutError = XsollaHttpRequestError(ConversionErrorMsg);
		}
		else
		{
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);

			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				if (!ParseError(JsonObject, OutError))
				{
					OutError = XsollaHttpRequestError(UnknownErrorMsg);
				}
			}
			else
			{
				OutError = XsollaHttpRequestError(DeserializationErrorMsg);
			}
		}
	}
	else
	{
		OutError = XsollaHttpRequestError(NoResponseErrorMsg);
	}

	return false;
}
