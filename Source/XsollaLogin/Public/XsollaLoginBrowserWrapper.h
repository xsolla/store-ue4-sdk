// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaLoginBrowserWrapper.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnLoginBrowserClosed, bool);

UCLASS()
class XSOLLALOGIN_API UXsollaLoginBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Login|Browser")
	void LoadUrl(const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login|Browser")
	void ExecuteBrowserClosed(bool bAuthenticationCompleted);

	FOnLoginBrowserClosed OnBrowserClosed;
};
