// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsHttpRequestHelper.h"
#include "XsollaUtilsDefines.h"

#include "Runtime/Launch/Resources/Version.h"

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> XsollaUtilsHttpRequestHelper::CreateHttpRequest(const FString& Url,
	const EXsollaHttpRequestVerb Verb,
	const FString& AuthToken,
	const FString& Content,
	const FString& SdkModuleName,
	const FString& SdkModuleVersion)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Xsolla analytics URL meta
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=%s&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		*SdkModuleName.ToLower(),
		*SdkModuleVersion);
	HttpRequest->SetURL(Url + MetaUrl);

	// Xsolla analytics header meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), SdkModuleName);
	HttpRequest->SetHeader(TEXT("X-SDK-V"), SdkModuleVersion);

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
