// Copyright 2022 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaStoreBrowserWrapper.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FOnPaymentSuccess);
DECLARE_DELEGATE_OneParam(FOnPaymentError, const FString&);
DECLARE_DELEGATE(FOnPaymentCancel);

UCLASS()
class XSOLLASTORE_API UXsollaStoreBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Xsolla|Store|Browser")
	void LoadUrl(const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Browser")
	void ExecuteSuccess();

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Browser")
	void ExecuteError(const FString& ErrorMessage);

	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store|Browser")
	void ExecuteCancel();
	
	FOnPaymentSuccess OnSuccess;

	FOnPaymentError OnError;

	FOnPaymentCancel OnCancel;
};
