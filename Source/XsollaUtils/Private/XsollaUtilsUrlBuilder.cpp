// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsUrlBuilder.h"

XsollaUtilsUrlBuilder::XsollaUtilsUrlBuilder(const FString& UrlTemplate)
	: Url(UrlTemplate)
{
}

FString XsollaUtilsUrlBuilder::Build()
{
	FString ResultUrl = Url;

	// set path params
	for (const auto& Param : PathParams)
	{
		const FString ParamPlaceholder = FString::Printf(TEXT("{%s}"), *Param.Key);
		if (ResultUrl.Contains(ParamPlaceholder))
		{
			ResultUrl = ResultUrl.Replace(*ParamPlaceholder, *Param.Value);
		}
	}

	// add query params
	for (const auto& Param : StringQueryParams)
	{
		const FString NewParam = FString::Printf(TEXT("%s%s=%s"), ResultUrl.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Param.Key, *Param.Value);
		ResultUrl.Append(NewParam);
	}

	for (const auto& Param : NumberQueryParams)
	{
		const FString NewParam = FString::Printf(TEXT("%s%s=%d"), ResultUrl.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Param.Key, Param.Value);
		ResultUrl.Append(NewParam);
	}

	return ResultUrl;
}

void XsollaUtilsUrlBuilder::SetPathParam(const FString& ParamName, const FString& ParamValue)
{
	PathParams.Add(TPair<FString, FString>(ParamName, ParamValue));
}

void XsollaUtilsUrlBuilder::AddStringQueryParam(const FString& ParamName, const FString& ParamValue, bool IgnoreEmpty)
{
	if (IgnoreEmpty && ParamValue.IsEmpty())
	{
		return;
	}

	StringQueryParams.Add(TPair<FString, FString>(ParamName, ParamValue));
}

void XsollaUtilsUrlBuilder::AddStringArrayQueryParam(const FString& ParamName, const TArray<FString>& ParamValueArray, bool IgnoreEmpty)
{
	for (const auto& Param : ParamValueArray)
	{
		AddStringArrayQueryParam(ParamName, ParamValueArray, IgnoreEmpty);
	}
}

void XsollaUtilsUrlBuilder::AddNumberQueryParam(const FString& ParamName, int32 ParamValue)
{
	NumberQueryParams.Add(TPair<FString, int32>(ParamName, ParamValue));
}
