// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreLibrary.h"

#include "XsollaStoreDataModel.h"

UXsollaStoreLibrary::UXsollaStoreLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UXsollaStoreLibrary::Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B)
{
	return A == B;
}
