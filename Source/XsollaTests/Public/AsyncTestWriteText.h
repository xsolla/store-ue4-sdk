// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTestWriteText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWriteTextOutputPin);

/**
 * 
 */
UCLASS()
class XSOLLATESTS_API UAsyncTestWriteText : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FWriteTextOutputPin AfterWriteText;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Flow Control")
    static UAsyncTestWriteText* WriteText(const UObject* WorldContextObject, const FString& Text);

	virtual void Activate() override;

private:
	const UObject* WorldContextObject;
	
	FString Text;

	UFUNCTION()
	void ExecuteAfterWriteText();

	FTimerHandle TimerHandle;
};
