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

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::SetPathParam(const FString& ParamName, const FString& ParamValue)
{
	PathParams.Add(TPair<FString, FString>(ParamName, ParamValue));

	return *this;
}

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::SetPathParam(const FString& ParamName, const int32 ParamValue)
{
	PathParams.Add(TPair<FString, FString>(ParamName, FString::FromInt(ParamValue)));

	return *this;
}

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::SetPathParam(const FString& ParamName, const int64 ParamValue)
{
	PathParams.Add(TPair<FString, FString>(ParamName, FString::Printf(TEXT("%llu"), ParamValue)));

	return *this;
}

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::AddStringQueryParam(const FString& ParamName, const FString& ParamValue, const bool IgnoreEmpty)
{
	if (IgnoreEmpty && ParamValue.IsEmpty())
	{
		return *this;
	}

	StringQueryParams.Add(TPair<FString, FString>(ParamName, ParamValue));

	return *this;
}

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::AddArrayQueryParam(const FString& ParamName, const TArray<FString>& ParamValueArray, const bool IgnoreEmpty, const bool AsOneParam)
{
	if (IgnoreEmpty && ParamValueArray.Num() == 0)
	{
		return *this;
	}

	if (AsOneParam)
	{
		FString AdditionalFieldsString = FString::Join(ParamValueArray, TEXT(","));
		AdditionalFieldsString.RemoveFromEnd(",");
		AddStringQueryParam(ParamName, AdditionalFieldsString, IgnoreEmpty);
	}
	else
	{
		for (const auto& Param : ParamValueArray)
		{
			AddStringQueryParam(ParamName, Param, IgnoreEmpty);
		}
	}

	return *this;
}

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::AddNumberQueryParam(const FString& ParamName, const int32 ParamValue)
{
	NumberQueryParams.Add(TPair<FString, int32>(ParamName, ParamValue));

	return *this;
}

XsollaUtilsUrlBuilder& XsollaUtilsUrlBuilder::AddBoolQueryParam(const FString& ParamName, const bool ParamValue, const bool AsNumber)
{
	if (AsNumber)
	{
		AddStringQueryParam(ParamName, ParamValue ? TEXT("1") : TEXT("0"), true);
	}
	else
	{
		AddStringQueryParam(ParamName, ParamValue ? TEXT("true") : TEXT("false"), true);
	}

	return *this;
}