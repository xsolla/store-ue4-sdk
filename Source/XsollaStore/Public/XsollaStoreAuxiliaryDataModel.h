// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaStoreDataModel.h"
#include "XsollaStoreDelegates.h"
#include "XsollaStoreAuxiliaryDataModel.generated.h"

USTRUCT()
struct FGetAllVirtualItemsParams
{
	GENERATED_BODY()

	UPROPERTY()
	FString Locale;

	UPROPERTY()
	FString Country;

	UPROPERTY()
	TArray<FString> AdditionalFields;

	UPROPERTY()
	FOnStoreItemsUpdate ResultSuccessCallback;

	UPROPERTY()
	FOnError ResultErrorCallback;

	UPROPERTY()
	FStoreItemsData ResultData;

	UPROPERTY()
	FErrorData ResultErrorData;

	UPROPERTY()
	FOnStoreItemsUpdate CurrentSuccessCallback;

	UPROPERTY()
	FOnError CurrentErrorCallback;

	UPROPERTY()
	int32 Limit = 50;

	UPROPERTY()
	int32 Offset = 0;

	UPROPERTY()
	FString AuthToken;

	void ProcessNextPartOfData(const FStoreItemsData& InItemsData, const TFunction<void()>& NextCallFunc)
	{
		ResultData.Items.Append(InItemsData.Items);

		if (InItemsData.has_more)
		{
			Offset += Limit;
			NextCallFunc();
		}
		else
		{
			Finish(true);
		}
	}

	void Finish(bool isSuccess)
	{
		CurrentSuccessCallback.Unbind();
		CurrentErrorCallback.Unbind();
		isSuccess ? ResultSuccessCallback.ExecuteIfBound(ResultData) : ResultErrorCallback.ExecuteIfBound(ResultErrorData.StatusCode, ResultErrorData.ErrorCode, ResultErrorData.ErrorMessage);
	}

	FGetAllVirtualItemsParams()
	{
	}

	FGetAllVirtualItemsParams(
		const FString& InLocale,
		const FString& InCountry,
		const TArray<FString>& InAdditionalFields,
		const FOnStoreItemsUpdate& InResultSuccessCallback,
		const FOnError& InResultErrorCallback,
		const FString& InAuthToken)
		: Locale(InLocale)
		, Country(InCountry)
		, AdditionalFields(InAdditionalFields)
		, ResultSuccessCallback(InResultSuccessCallback)
		, ResultErrorCallback(InResultErrorCallback)
		, AuthToken(InAuthToken)
	{
	}
};

USTRUCT()
struct FGetAllVirtualCurrenciesParams
{
	GENERATED_BODY()

	UPROPERTY()
	FString Locale;

	UPROPERTY()
	FString Country;

	UPROPERTY()
	TArray<FString> AdditionalFields;

	UPROPERTY()
	FOnVirtualCurrenciesUpdate ResultSuccessCallback;

	UPROPERTY()
	FOnError ResultErrorCallback;

	UPROPERTY()
	FVirtualCurrencyData ResultData;

	UPROPERTY()
	FErrorData ResultErrorData;

	UPROPERTY()
	FOnVirtualCurrenciesUpdate CurrentSuccessCallback;

	UPROPERTY()
	FOnError CurrentErrorCallback;

	UPROPERTY()
	int32 Limit = 50;

	UPROPERTY()
	int32 Offset = 0;

	void ProcessNextPartOfData(const FVirtualCurrencyData& InCurrenciesData, const TFunction<void()>& NextCallFunc)
	{
		ResultData.Items.Append(InCurrenciesData.Items);

		if (InCurrenciesData.has_more)
		{
			Offset += Limit;
			NextCallFunc();
		}
		else
		{
			Finish(true);
		}
	}

	void Finish(bool isSuccess)
	{
		CurrentSuccessCallback.Unbind();
		CurrentErrorCallback.Unbind();
		isSuccess ? ResultSuccessCallback.ExecuteIfBound(ResultData) : ResultErrorCallback.ExecuteIfBound(ResultErrorData.StatusCode, ResultErrorData.ErrorCode, ResultErrorData.ErrorMessage);
	}

	FGetAllVirtualCurrenciesParams()
	{
	}

	FGetAllVirtualCurrenciesParams(
		const FString& InLocale,
		const FString& InCountry,
		const TArray<FString>& InAdditionalFields,
		const FOnVirtualCurrenciesUpdate& InResultSuccessCallback,
		const FOnError& InResultErrorCallback)
		: Locale(InLocale)
		, Country(InCountry)
		, AdditionalFields(InAdditionalFields)
		, ResultSuccessCallback(InResultSuccessCallback)
		, ResultErrorCallback(InResultErrorCallback)
	{
	}
};

USTRUCT()
struct FGetAllVirtualCurrencyPackagesParams
{
	GENERATED_BODY()

	UPROPERTY()
	FString Locale;

	UPROPERTY()
	FString Country;

	UPROPERTY()
	TArray<FString> AdditionalFields;

	UPROPERTY()
	FOnVirtualCurrencyPackagesUpdate ResultSuccessCallback;

	UPROPERTY()
	FOnError ResultErrorCallback;

	UPROPERTY()
	FVirtualCurrencyPackagesData ResultData;

	UPROPERTY()
	FErrorData ResultErrorData;

	UPROPERTY()
	FOnVirtualCurrencyPackagesUpdate CurrentSuccessCallback;

	UPROPERTY()
	FOnError CurrentErrorCallback;

	UPROPERTY()
	int32 Limit = 50;

	UPROPERTY()
	int32 Offset = 0;

	UPROPERTY()
	FString AuthToken;

	void ProcessNextPartOfData(const FVirtualCurrencyPackagesData& InCurrencyPackagesData, const TFunction<void()>& NextCallFunc)
	{
		ResultData.Items.Append(InCurrencyPackagesData.Items);

		if (InCurrencyPackagesData.has_more)
		{
			Offset += Limit;
			NextCallFunc();
		}
		else
		{
			Finish(true);
		}
	}

	void Finish(bool isSuccess)
	{
		CurrentSuccessCallback.Unbind();
		CurrentErrorCallback.Unbind();
		isSuccess ? ResultSuccessCallback.ExecuteIfBound(ResultData) : ResultErrorCallback.ExecuteIfBound(ResultErrorData.StatusCode, ResultErrorData.ErrorCode, ResultErrorData.ErrorMessage);
	}

	FGetAllVirtualCurrencyPackagesParams()
	{
	}

	FGetAllVirtualCurrencyPackagesParams(
		const FString& InLocale,
		const FString& InCountry,
		const TArray<FString>& InAdditionalFields,
		const FOnVirtualCurrencyPackagesUpdate& InResultSuccessCallback,
		const FOnError& InResultErrorCallback,
		const FString& InAuthToken)
		: Locale(InLocale)
		, Country(InCountry)
		, AdditionalFields(InAdditionalFields)
		, ResultSuccessCallback(InResultSuccessCallback)
		, ResultErrorCallback(InResultErrorCallback)
		, AuthToken(InAuthToken)
	{
	}
};
