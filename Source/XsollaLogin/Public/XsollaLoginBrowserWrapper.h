// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaLoginBrowserWrapper.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginBrowserClosed, bool, bAuthenticationCompleted);

UCLASS()
class XSOLLALOGIN_API UXsollaLoginBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Login|Browser")
	void LoadUrl(const FString& Url);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLoginBrowserClosed OnBrowserClosed;
};
