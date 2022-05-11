// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaWebBrowserWrapper.generated.h"

/**
 * 
 */
UCLASS()
class XSOLLAWEBBROWSER_API UXsollaWebBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Browser")
	void LoadUrl(const FString& Url);
};
