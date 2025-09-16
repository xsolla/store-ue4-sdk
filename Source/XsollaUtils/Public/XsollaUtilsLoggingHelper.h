// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Logging/LogMacros.h"
#include "XsollaUtilsHttpRequestHelper.h"

/**
 * Helper class for logging across Xsolla modules
 */
class XSOLLAUTILS_API XsollaUtilsLoggingHelper
{
public:
    /** Sanitation configuration flags */
    static const bool bSanitizeAccessTokens;
    static const bool bSanitizeRefreshTokens;
    static const bool bSanitizePasswords;
    static const bool bSanitizeAuthHeaders;
    static const bool bSanitizeUrlsInResponses;
    static const bool bSanitizeUrls;
    static const int32 MaxContentLength;

    /** Converts HTTP verb enum to string */
    static FString VerbToString(const EXsollaHttpRequestVerb Verb);

    /** Sanitizes content to hide sensitive information */
    static FString SanitizeContentForLogging(const FString& Content);

    /** Sanitizes URL by hiding query parameters for logging purposes */
    static FString SanitizeUrlForLogging(const FString& Url);

    /** Logs URL */
    static void LogUrl(const FString& Url, const FString& Reason);

    /** Logs HTTP request details */
    static void LogHttpRequest(const FHttpRequestPtr& Request, const FLogCategoryBase& LogCat, const FString& Content = TEXT(""));

    /** Logs HTTP response details */
    static void LogHttpResponse(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, const FLogCategoryBase& LogCat);
};
