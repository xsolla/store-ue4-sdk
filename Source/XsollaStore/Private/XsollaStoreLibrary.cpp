// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreLibrary.h"

#include "XsollaStore.h"

#include "XsollaStoreDataModel.h"

UXsollaStoreLibrary::UXsollaStoreLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaStoreSettings* UXsollaStoreLibrary::GetStoreSettings()
{
	return FXsollaStoreModule::Get().GetSettings();
}

bool UXsollaStoreLibrary::Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B)
{
	return A == B;
}

FDateTime UXsollaStoreLibrary::MakeDateTimeFromTimestamp(int64 time)
{
	return FDateTime::FromUnixTimestamp(time);
}
