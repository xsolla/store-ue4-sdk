// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class XSOLLAUTILS_API XsollaUtilsUrlBuilder
{
public:
	/*
	 * Path params of input URL should be wrapped with curly braces in order to be processed properly
	 */
	XsollaUtilsUrlBuilder(const FString& UrlTemplate);

	FString Build();

	XsollaUtilsUrlBuilder& SetPathParam(const FString& ParamName, const FString& ParamValue);
	XsollaUtilsUrlBuilder& SetPathParam(const FString& ParamName, const int32 ParamValue);
	XsollaUtilsUrlBuilder& SetPathParam(const FString& ParamName, const int64 ParamValue);

	XsollaUtilsUrlBuilder& AddStringQueryParam(const FString& ParamName, const FString& ParamValue, const bool IgnoreEmpty = true);
	XsollaUtilsUrlBuilder& AddArrayQueryParam(const FString& ParamName, const TArray<FString>& ParamValueArray, const bool IgnoreEmpty = true, const bool AsOneParam = false);
	XsollaUtilsUrlBuilder& AddNumberQueryParam(const FString& ParamName, const int32 ParamValue);
	XsollaUtilsUrlBuilder& AddBoolQueryParam(const FString& ParamName, const bool ParamValue, const bool AsNumber = true);

private:
	FString Url;

	TArray<TPair<FString, FString>> PathParams;

	TArray<TPair<FString, FString>> StringQueryParams;
	TArray<TPair<FString, int32>> NumberQueryParams;
};