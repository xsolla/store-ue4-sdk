// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "XsollaUtilsDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLAUTILS_API FStoreGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	int32 id;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString external_id;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	int32 level;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	int32 order;

	// https://github.com/xsolla/store-ue4-sdk/issues/103
	//UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	//TArray<int32> children;

	UPROPERTY(BlueprintReadOnly, Category = "Item Group")
	FString parent_external_id;

	public:
	FStoreGroup()
        : id(-1)
        , level(0)
        , order(1){};
};