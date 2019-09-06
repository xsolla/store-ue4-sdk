// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreLibrary.h"

#include "XsollaStore.h"
#include "XsollaStoreController.h"
#include "XsollaStoreCurrencyFormat.h"
#include "XsollaStoreDataModel.h"

#include "Engine/Engine.h"
#include "Kismet/KismetTextLibrary.h"

UXsollaStoreLibrary::UXsollaStoreLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaStoreController* UXsollaStoreLibrary::GetStoreController(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return FXsollaStoreModule::Get().GetStoreController(World);
	}

	return nullptr;
}

UXsollaStoreSettings* UXsollaStoreLibrary::GetStoreSettings()
{
	return FXsollaStoreModule::Get().GetSettings();
}

void UXsollaStoreLibrary::LoadImageFromWeb(UObject* WorldContextObject, const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	auto StoreController = UXsollaStoreLibrary::GetStoreController(WorldContextObject);
	StoreController->GetImageLoader()->LoadImage(URL, SuccessCallback, ErrorCallback);
}

bool UXsollaStoreLibrary::Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B)
{
	return A == B;
}

FString UXsollaStoreLibrary::FormatPrice(UObject* WorldContextObject, float Amount, const FString& Currency)
{
	UDataTable* CurrencyLibrary = UXsollaStoreLibrary::GetStoreController(WorldContextObject)->GetCurrencyLibrary();
	check(CurrencyLibrary);

	auto Row = CurrencyLibrary->FindRow<FXsollaStoreCurrency>(FName(*Currency), FString());
	if (Row)
	{
		FString SanitizedAmount = UKismetTextLibrary::Conv_FloatToText(Amount, ERoundingMode::HalfToEven, false, true, 1, 324, Row->fractionSize, Row->fractionSize).ToString();
		return Row->symbol.format.Replace(TEXT("$"), *Row->symbol.grapheme).Replace(TEXT("1"), *SanitizedAmount);
	}

	return FString();
}
