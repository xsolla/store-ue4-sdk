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

	void SetPathParam(const FString& ParamName, const FString& ParamValue);

	void AddStringQueryParam(const FString& ParamName, const FString& ParamValue, bool IgnoreEmpty = true);
	void AddStringArrayQueryParam(const FString& ParamName, const TArray<FString>& ParamValueArray, bool IgnoreEmpty = true);
	void AddNumberQueryParam(const FString& ParamName, int32 ParamValue);

private:
	FString Url;

	TArray<TPair<FString, FString>> PathParams;

	TArray<TPair<FString, FString>> StringQueryParams;
	TArray<TPair<FString, int32>> NumberQueryParams;
};