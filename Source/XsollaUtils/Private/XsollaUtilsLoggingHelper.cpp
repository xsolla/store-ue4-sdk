// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsLoggingHelper.h"
#include "Internationalization/Regex.h"
#include "Logging/LogCategory.h"
#include "XsollaUtilsDefines.h"

// Initialize constants
const bool XsollaUtilsLoggingHelper::bSanitizeAccessTokens = true;
const bool XsollaUtilsLoggingHelper::bSanitizeRefreshTokens = true;
const bool XsollaUtilsLoggingHelper::bSanitizePasswords = true;
const bool XsollaUtilsLoggingHelper::bSanitizeAuthHeaders = true;
const bool XsollaUtilsLoggingHelper::bSanitizeUrlsInResponses = true;
const bool XsollaUtilsLoggingHelper::bSanitizeUrls = true;
const int32 XsollaUtilsLoggingHelper::MaxContentLength = 1024;

FString XsollaUtilsLoggingHelper::VerbToString(const EXsollaHttpRequestVerb Verb)
{
    switch (Verb)
    {
    case EXsollaHttpRequestVerb::VERB_GET:
        return TEXT("GET");
    case EXsollaHttpRequestVerb::VERB_POST:
        return TEXT("POST");
    case EXsollaHttpRequestVerb::VERB_PUT:
        return TEXT("PUT");
    case EXsollaHttpRequestVerb::VERB_DELETE:
        return TEXT("DELETE");
    case EXsollaHttpRequestVerb::VERB_PATCH:
        return TEXT("PATCH");
    default:
        return TEXT("UNKNOWN");
    }
}

FString XsollaUtilsLoggingHelper::SanitizeContentForLogging(const FString& Content)
{
    if (Content.IsEmpty())
    {
        return TEXT("<empty>");
    }

    // Create a copy to modify
    FString SanitizedContent = Content;

    // Define patterns to sanitize (using simple string replacement for compatibility)
    TArray<TPair<FString, FString>> SanitizationPatterns;

    // JWT tokens (partial masking)
    if (bSanitizeAccessTokens)
    {
        SanitizationPatterns.Add({TEXT("\"access_token\":\"([^\"]{5})[^\"]*\""), TEXT("\"access_token\":\"$1...\"")});
    }

    if (bSanitizeRefreshTokens)
    {
        SanitizationPatterns.Add({TEXT("\"refresh_token\":\"([^\"]{5})[^\"]*\""), TEXT("\"refresh_token\":\"$1...\"")});
    }

    // Passwords
    if (bSanitizePasswords)
    {
        SanitizationPatterns.Add({TEXT("\"password\":\"[^\"]*\""), TEXT("\"password\":\"***\"")});
    }

    // Auth tokens in headers
    if (bSanitizeAuthHeaders)
    {
        SanitizationPatterns.Add({TEXT("Bearer ([^\"]{5})[^\"]*"), TEXT("Bearer $1...")});
    }

    // URLs in response bodies
    if (bSanitizeUrlsInResponses)
    {
        SanitizationPatterns.Add({TEXT("\"url\":\"([^\"]+\\?)([^\"]*)\""), TEXT("\"url\":\"$1[query_params_hidden]\"")});
    }

    // Apply each sanitization pattern
    for (const auto& Pattern : SanitizationPatterns)
    {
        FRegexPattern RegexPattern(Pattern.Key);
        FRegexMatcher Matcher(RegexPattern, SanitizedContent);

        while (Matcher.FindNext())
        {
            FString Replacement = Pattern.Value;
            if (Replacement.Contains(TEXT("$1")))
            {
                Replacement = Replacement.Replace(TEXT("$1"), *Matcher.GetCaptureGroup(1));
            }

            SanitizedContent = SanitizedContent.Replace(*Matcher.GetCaptureGroup(0), *Replacement);
        }
    }

    // If content is too long, truncate it
    if (SanitizedContent.Len() > MaxContentLength)
    {
        SanitizedContent = SanitizedContent.Left(MaxContentLength) + TEXT("... [truncated]");
    }

    return SanitizedContent;
}

FString XsollaUtilsLoggingHelper::SanitizeUrlForLogging(const FString& Url)
{
    FString SanitizedUrl = Url;

    if (bSanitizeUrls)
    {
        int32 QueryPos = Url.Find(TEXT("?"));
        if (QueryPos != INDEX_NONE)
        {
            SanitizedUrl = Url.Left(QueryPos + 1) + TEXT("[query_params_hidden]");
        }
    }

    return SanitizedUrl;
}

void XsollaUtilsLoggingHelper::LogUrl(const FString& Url, const FString& Reason)
{
    FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(Url);
    UE_LOG(LogXsollaUtils, Log, TEXT("URL: %s, Reason: %s"), *SanitizedUrl, *Reason);
}

void XsollaUtilsLoggingHelper::LogHttpRequest(const FHttpRequestPtr& Request, const FLogCategoryBase& LogCat, const FString& Content)
{
    if (!Request.IsValid())
    {
        return;
    }

    UE_LOG(LogXsollaUtils, Log, TEXT("HTTP Request - URL: %s, Verb: %s"),
           *Request->GetURL(), *Request->GetVerb());

    // Log headers (excluding Authorization which may contain sensitive data)
    FString Headers;
    TArray<FString> AllHeaders = Request->GetAllHeaders();
    for (int32 i = 0; i < AllHeaders.Num(); ++i)
    {
        const FString& HeaderStr = AllHeaders[i];
        FString Key, Value;
        if (HeaderStr.Split(TEXT(": "), &Key, &Value))
        {
            if (!Key.Contains(TEXT("Authorization")))
            {
                Headers += FString::Printf(TEXT("%s: %s; "), *Key, *Value);
            }
            else
            {
                Headers += FString::Printf(TEXT("%s: [HIDDEN]; "), *Key);
            }
        }
    }

    if (!Headers.IsEmpty())
    {
        UE_LOG(LogXsollaUtils, Log, TEXT("HTTP Request Headers: %s"), *Headers);
    }

    // Log content if provided and not empty
    if (!Content.IsEmpty())
    {
        UE_LOG(LogXsollaUtils, Log, TEXT("HTTP Request Content: %s"),
               *SanitizeContentForLogging(Content));
    }
}

void XsollaUtilsLoggingHelper::LogHttpResponse(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, const FLogCategoryBase& LogCat)
{
    if (!Request.IsValid() || !Response.IsValid())
    {
        return;
    }

    // Log response code and URL
    UE_LOG(LogXsollaUtils, Log, TEXT("HTTP Response - Code: %d, URL: %s"),
           Response->GetResponseCode(), *Request->GetURL());

    // Log response content if available
    FString ResponseContent = Response->GetContentAsString();
    if (!ResponseContent.IsEmpty())
    {
        UE_LOG(LogXsollaUtils, Log, TEXT("HTTP Response Content: %s"),
               *SanitizeContentForLogging(ResponseContent));
    }
}
