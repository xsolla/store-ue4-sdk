// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once
#include "GameFramework/SaveGame.h"
#include "XsollaLoginTypes.h"
#include "XsollaDevMenuSave.generated.h"


UCLASS()
class UXsollaDevMenuSave : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FXsollaDevMenuData LoadDevMenuData();

	UFUNCTION(BlueprintCallable)
	static void SaveDevMenuData(const FString& InLoginId, const FString& InProjectId, const bool bInUseOAuth2, const FString& InClientId);

public:
	static const FString SaveSlotName;

	/** User index (always 0) */
	static const int32 UserIndex;

protected:
	UPROPERTY()
	FXsollaDevMenuData DevMenuData;
};
