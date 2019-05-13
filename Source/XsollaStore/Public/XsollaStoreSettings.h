// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaStoreSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLASTORE_API UXsollaStoreSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Store ID from Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere)
	FString ProjectId;

	/** If yes, external (system) browser will be launched to process payments */
	UPROPERTY(Config, EditAnywhere)
	bool bUsePlatformBrowser;
};
