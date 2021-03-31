// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "XsollaDemoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class XSOLLAUTILS_API AXsollaDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default|Custom", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LoginDemo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default|Custom", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> StoreDemo;

public:
	UFUNCTION(BlueprintPure, Category = "XsollaDemoGameModeBase")
	TSubclassOf<UUserWidget> GetLoginDemo() const;

	UFUNCTION(BlueprintPure, Category = "XsollaDemoGameModeBase")
	TSubclassOf<UUserWidget> GetStoreDemo() const;
};
