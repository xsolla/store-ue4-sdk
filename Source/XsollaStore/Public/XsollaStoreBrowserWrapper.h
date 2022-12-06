// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaStoreBrowserWrapper.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FOnBrowserClosed);

UCLASS()
class XSOLLASTORE_API UXsollaStoreBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Store|Browser")
	void LoadUrl(const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Browser")
	void ExecuteBrowserClosed();
	
	FOnBrowserClosed OnBrowserClosed;
};
