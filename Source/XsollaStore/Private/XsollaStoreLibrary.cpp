// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreLibrary.h"
#include "XsollaStore.h"
#include "XsollaStoreCurrencyFormat.h"
#include "XsollaStoreDataModel.h"

#include "Engine/DataTable.h"
#include "Kismet/KismetTextLibrary.h"
#include "UObject/ConstructorHelpers.h"

UDataTable* UXsollaStoreLibrary::CurrencyLibrary;

UXsollaStoreLibrary::UXsollaStoreLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CurrencyLibraryObj(
		TEXT("DataTable'/Xsolla/Store/Misc/currency-format.currency-format'"));
	CurrencyLibrary = CurrencyLibraryObj.Object;
}

UXsollaStoreSettings* UXsollaStoreLibrary::GetStoreSettings()
{
	return FXsollaStoreModule::Get().GetSettings();
}

bool UXsollaStoreLibrary::Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B)
{
	return A == B;
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_IntToXsollaJsonVariant(int Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_FloatToXsollaJsonVariant(float Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_StringToXsollaJsonVariant(const FString& Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_BoolToXsollaJsonVariant(bool Value)
{
	return FXsollaJsonVariant(Value);
}

UDataTable* UXsollaStoreLibrary::GetCurrencyLibrary()
{
	return CurrencyLibrary;
}

FString UXsollaStoreLibrary::FormatPrice(float Amount, const FString& Currency /*= TEXT("USD")*/)
{
	if (Currency.IsEmpty())
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: In PA there is no price provided for certain item"), *VA_FUNC_LINE);
		return FString();
	}

	auto Row = GetCurrencyLibrary()->FindRow<FXsollaStoreCurrency>(FName(*Currency), FString());
	if (Row)
	{
		FString SanitizedAmount = UKismetTextLibrary::Conv_FloatToText(Amount, ERoundingMode::HalfToEven,
			false, true, 1, 324, Row->fractionSize, Row->fractionSize)
									  .ToString();
		return Row->symbol.format.Replace(TEXT("$"), *Row->symbol.grapheme).Replace(TEXT("1"), *SanitizedAmount);
	}

	UE_LOG(LogXsollaStore, Error, TEXT("%s: Failed to format price (%d %s)"), *VA_FUNC_LINE, Amount, *Currency);
	return FString();
}
