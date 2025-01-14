// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaSocialLinkingBrowserWrapper.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_FourParams(FOnSocialLinkingBrowserClosed, bool bIsManually, const FString& Token, const FString& ErrorCode, const FString& ErrorDescription);

UCLASS()
class XSOLLALOGIN_API UXsollaSocialLinkingBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Login|Browser")
	void LoadUrl(const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login|Browser")
	void ExecuteBrowserClosed(bool bIsManually, const FString& Token, const FString& ErrorCode, const FString& ErrorDescription);

	FOnSocialLinkingBrowserClosed OnBrowserClosed;
};
