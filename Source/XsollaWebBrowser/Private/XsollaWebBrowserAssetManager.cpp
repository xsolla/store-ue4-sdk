// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaWebBrowserAssetManager.h"

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
#include "WebBrowserTexture.h"
#endif

UXsollaWebBrowserAssetManager::UXsollaWebBrowserAssetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultMaterial(FString(TEXT("/Xsolla/Common/Browser/M_WebTexture.M_WebTexture")))
	, DefaultTranslucentMaterial(FString(TEXT("/Xsolla/Common/Browser/M_WebTexture_Translucent.M_WebTexture_Translucent")))
{
#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
	// Add a hard reference to UXsollaWebBrowserTexture, without this the WebBrowserTexture DLL never gets loaded on Windows.
	UWebBrowserTexture::StaticClass();
#endif
};

void UXsollaWebBrowserAssetManager::LoadDefaultMaterials()
{
	DefaultMaterial.LoadSynchronous();
	DefaultTranslucentMaterial.LoadSynchronous();
}

UMaterial* UXsollaWebBrowserAssetManager::GetDefaultMaterial() const
{
	return DefaultMaterial.Get();
}

UMaterial* UXsollaWebBrowserAssetManager::GetDefaultTranslucentMaterial() const
{
	return DefaultTranslucentMaterial.Get();
}
