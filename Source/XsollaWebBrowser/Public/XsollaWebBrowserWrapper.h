// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaWebBrowserWrapper.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FOnBrowserClosed);

UCLASS()
class XSOLLAWEBBROWSER_API UXsollaWebBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Browser")
	void LoadUrl(const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Browser")
	void BrowserClosed();
	
	FOnBrowserClosed OnBrowserClosed;
};
