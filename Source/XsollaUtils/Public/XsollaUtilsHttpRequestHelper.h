// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"

enum class EXsollaHttpRequestVerb : uint8
{
	VERB_GET,
	VERB_POST,
	VERB_PUT,
	VERB_DELETE,
	VERB_PATCH
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
};