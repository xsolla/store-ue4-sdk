// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsHttpRequestHelper.h"
#include "XsollaUtilsDefines.h"
#include "XsollaUtilsLibrary.h"

#include "Dom/JsonObject.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

const FString XsollaUtilsHttpRequestHelper::NoResponseErrorMsg(TEXT("No response"));
const FString XsollaUtilsHttpRequestHelper::UnknownErrorMsg(TEXT("Unknown error"));
const FString XsollaUtilsHttpRequestHelper::DeserializationErrorMsg(TEXT("Failed to deserialize response"));
const FString XsollaUtilsHttpRequestHelper::ConversionErrorMsg(TEXT("Failed to convert response"));

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> XsollaUtilsHttpRequestHelper::CreateHttpRequest(const FString& Url,
	const EXsollaHttpRequestVerb Verb,
	const FString& AuthToken,
	const FString& Content,
	const FString& SdkModuleName,
	const FString& SdkModuleVersion)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Referral analytics meta
	FString XRef = TEXT("");
	FString XRefV = TEXT("");

	UXsollaUtilsLibrary::GetPartnerInfo(XRef, XRefV);

	bool IsReferralAnalyticsSet = !XRef.IsEmpty() && !XRefV.IsEmpty();

	// Xsolla analytics URL meta
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=%s&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		*SdkModuleName.ToLower(),
		*SdkModuleVersion);

	// Referral analytics URL meta
	const FString ReferralMetaUrl = IsReferralAnalyticsSet ? FString::Printf(TEXT("&ref=%s&ref_v=%s"), *XRef.ToLower(), *XRefV.ToLower()) : TEXT("");

	HttpRequest->SetURL(Url + MetaUrl + ReferralMetaUrl);

	// Xsolla analytics header meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), SdkModuleName);
	HttpRequest->SetHeader(TEXT("X-SDK-V"), SdkModuleVersion);

	// Referral analytics header meta
	if (IsReferralAnalyticsSet)
	{
		HttpRequest->SetHeader(TEXT("X-REF"), XRef);
		HttpRequest->SetHeader(TEXT("X-REF-V"), XRefV);
	}

	switch (Verb)
	{
	case EXsollaHttpRequestVerb::VERB_GET:
		HttpRequest->SetVerb(TEXT("GET"));
		break;

	case EXsollaHttpRequestVerb::VERB_POST:
		HttpRequest->SetVerb(TEXT("POST"));
		break;

	case EXsollaHttpRequestVerb::VERB_PUT:
		HttpRequest->SetVerb(TEXT("PUT"));
		break;

	case EXsollaHttpRequestVerb::VERB_DELETE:
		HttpRequest->SetVerb(TEXT("DELETE"));
		break;

	case EXsollaHttpRequestVerb::VERB_PATCH:
		HttpRequest->SetVerb(TEXT("PATCH"));
		break;

	default:
		unimplemented();
	}

	if (!AuthToken.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));
	}

	if (!Content.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		HttpRequest->SetContentAsString(Content);
	}

	return HttpRequest;
}

bool XsollaUtilsHttpRequestHelper::ParseResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, XsollaHttpRequestError& OutError)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			return true;
		}
		else
		{
			FString ResponseStr = HttpResponse->GetContentAsString();

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

bool XsollaUtilsHttpRequestHelper::ParseResponseAsJson(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, TSharedPtr<FJsonObject>& OutResponse, XsollaHttpRequestError& OutError)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString ResponseStr = HttpResponse->GetContentAsString();

		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
			{
				OutResponse = JsonObject;
				return true;
			}
			else
			{
				if (!ParseError(JsonObject, OutError))
				{
					OutError = XsollaHttpRequestError(UnknownErrorMsg);
				}
			}
		}
		else
		{
			OutError = XsollaHttpRequestError(DeserializationErrorMsg);
		}
	}
	else
	{
		OutError = XsollaHttpRequestError(NoResponseErrorMsg);
	}

	return false;
}

bool XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, const UStruct* OutResponseDefinition, void* OutResponse, XsollaHttpRequestError& OutError)
{
	TSharedPtr<FJsonObject> JsonObject;
	if (ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), OutResponseDefinition, OutResponse))
		{
			return true;
		}

		OutError = XsollaHttpRequestError(ConversionErrorMsg);
	}

	return false;
}

bool XsollaUtilsHttpRequestHelper::ParseError(TSharedPtr<FJsonObject> JsonObject, XsollaHttpRequestError& OutError)
{
	// Try parse Login API error
	if (JsonObject->HasTypedField<EJson::Object>(TEXT("error")))
	{
		TSharedPtr<FJsonObject> ErrorObject = JsonObject.Get()->GetObjectField(TEXT("error"));
		OutError.code = ErrorObject.Get()->GetStringField(TEXT("code"));
		OutError.description = ErrorObject.Get()->GetStringField(TEXT("description"));
		return true;
	}

	// Try parse Commerce API error
	if (JsonObject->HasTypedField<EJson::String>(TEXT("errorMessage")))
	{
		OutError.statusCode = JsonObject->GetNumberField(TEXT("statusCode"));
		OutError.errorCode = JsonObject->GetNumberField(TEXT("errorCode"));
		OutError.errorMessage = JsonObject->GetStringField(TEXT("errorMessage"));
		return true;
	}

	// No error
	return false;
}
