// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Materials/Material.h"
#include "UObject/SoftObjectPtr.h"

#include "XsollaWebBrowserAssetManager.generated.h"

class UMaterial;

UCLASS()
class XSOLLAWEBBROWSER_API UXsollaWebBrowserAssetManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void LoadDefaultMaterials();

	UMaterial* GetDefaultMaterial() const;
	UMaterial* GetDefaultTranslucentMaterial() const;

protected:
	UPROPERTY()
	TSoftObjectPtr<UMaterial> DefaultMaterial;

	UPROPERTY()
	TSoftObjectPtr<UMaterial> DefaultTranslucentMaterial;
};
