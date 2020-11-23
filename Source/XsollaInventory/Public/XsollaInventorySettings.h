// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUtilsDataModel.h"

#include "XsollaInventorySettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class XSOLLAINVENTORY_API UXsollaInventorySettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Project ID from your Publisher Account. Required. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Inventory Settings")
	FString ProjectID;

	/** If enabled, Inventory SDK will imitate platform-specific requests so you can try account linking from different platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, DisplayName = "Use Cross-Platform Account Linking", Category = "Xsolla Inventory Settings")
	bool UseCrossPlatformAccountLinking;

	/** Target platform for cross-platform account linking. If using Xsolla Login, make sure that in the Login settings the same platform is chosen. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Inventory Settings", meta = (EditCondition = "UseCrossPlatformAccountLinking"))
	EXsollaPublishingPlatform Platform;

	/** Web Store URL to be opened in order to purchase virtual items. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Inventory Demo")
	FString WebStoreURL;

	/** Demo Project ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Inventory Demo")
	FString DemoProjectID;
};
