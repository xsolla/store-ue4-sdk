// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Http.h"
#include "XsollaUtils/Public/XsollaUtilsHttpRequestHelper.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXsollaLogin, Log, All);

#define VA_FUNC (FString(__FUNCTION__))				 // Current Class Name + Function Name where this is called
#define VA_LINE (FString::FromInt(__LINE__))		 // Current Line Number in the code where this is called
#define VA_FUNC_LINE (VA_FUNC + "(" + VA_LINE + ")") // Current Class and Line Number where this is called!

#define XSOLLA_LOGIN_VERSION TEXT("4.0.0")

/** Utility functions for logging */
namespace XsollaLoginLogging
{
    /** Converts HTTP verb enum to string */
    inline FString VerbToString(const EXsollaHttpRequestVerb Verb)
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

    /** Sanitizes content to hide sensitive information */
    inline FString SanitizeContentForLogging(const FString& Content)
    {
        if (Content.IsEmpty())
        {
            return TEXT("<empty>");
        }

        // Create a copy to modify
        FString SanitizedContent = Content;

        // Define patterns to sanitize (using simple string replacement for compatibility)
        TArray<TPair<FString, FString>> SanitizationPatterns = {
            // JWT tokens (partial masking)
            {TEXT("\"access_token\":\"([^\"]{5})[^\"]*\""), TEXT("\"access_token\":\"$1...\"")},
            {TEXT("\"refresh_token\":\"([^\"]{5})[^\"]*\""), TEXT("\"refresh_token\":\"$1...\"")},
            // Passwords
            {TEXT("\"password\":\"[^\"]*\""), TEXT("\"password\":\"***\"")},
            // Auth tokens in headers
            {TEXT("Bearer ([^\"]{5})[^\"]*"), TEXT("Bearer $1...")}
        };

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
        const int32 MaxContentLength = 1024;
        if (SanitizedContent.Len() > MaxContentLength)
        {
            SanitizedContent = SanitizedContent.Left(MaxContentLength) + TEXT("... [truncated]");
        }

        return SanitizedContent;
    }

    /** Logs HTTP request details */
    inline void LogHttpRequest(const FHttpRequestPtr& Request, const FString& Content = TEXT(""))
    {
        if (!Request.IsValid())
        {
            return;
        }

        // Log URL and verb
        UE_LOG(LogXsollaLogin, Verbose, TEXT("HTTP Request - URL: %s, Verb: %s"),
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
            UE_LOG(LogXsollaLogin, Verbose, TEXT("HTTP Request Headers: %s"), *Headers);
        }

        // Log content if provided and not empty
        if (!Content.IsEmpty())
        {
            UE_LOG(LogXsollaLogin, Verbose, TEXT("HTTP Request Content: %s"),
                   *SanitizeContentForLogging(Content));
        }
    }

    /** Logs HTTP response details */
    inline void LogHttpResponse(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response)
    {
        if (!Request.IsValid() || !Response.IsValid())
        {
            return;
        }

        // Log response code and URL
        UE_LOG(LogXsollaLogin, Verbose, TEXT("HTTP Response - Code: %d, URL: %s"),
               Response->GetResponseCode(), *Request->GetURL());

        // Log response content if available
        FString ResponseContent = Response->GetContentAsString();
        if (!ResponseContent.IsEmpty())
        {
            UE_LOG(LogXsollaLogin, Verbose, TEXT("HTTP Response Content: %s"),
                   *SanitizeContentForLogging(ResponseContent));
        }
    }
}
