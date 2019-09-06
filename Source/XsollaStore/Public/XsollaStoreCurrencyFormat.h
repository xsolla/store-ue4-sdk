// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreDefines.h"

#include "Engine/DataTable.h"

#include "XsollaStoreCurrencyFormat.generated.h"

USTRUCT(Blueprintable)
struct XSOLLASTORE_API FXsollaStoreCurrencySymbol
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString grapheme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString format;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool rtl;

	FXsollaStoreCurrencySymbol()
		: rtl(false){};
};

USTRUCT(Blueprintable)
struct XSOLLASTORE_API FXsollaStoreCurrency : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 fractionSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FXsollaStoreCurrencySymbol symbol;

	FXsollaStoreCurrency()
		: fractionSize(0)
	{
	}
};
