// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XsollaStoreDataModel.h"
#include "XsollaStoreDelegates.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreItemsUpdate, const FStoreItemsData&, ItemsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVirtualCurrenciesUpdate, const FVirtualCurrencyData&, VirtualCurrencyData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVirtualCurrencyPackagesUpdate, const FVirtualCurrencyPackagesData&, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetItemsListBySpecifiedGroup, const FStoreItemsList&, ItemsList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetListOfBundlesUpdate, const FStoreListOfBundles&, ListOfBundles);

UCLASS()
class XSOLLASTORE_API UXsollaStoreDelegates : public UObject
{
	GENERATED_BODY()
};