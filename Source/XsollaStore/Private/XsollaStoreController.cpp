// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreController.h"

#include "XsollaStore.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreSettings.h"

#include "Json.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Launch/Resources/Version.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

UXsollaStoreController::UXsollaStoreController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#undef LOCTEXT_NAMESPACE
