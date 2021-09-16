// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowserAssetManager.h"

#include "XsollaUtilsLibrary.h"
#include "XsollaWebBrowserModule.h"

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
#include "WebBrowserTexture.h"
#endif

UXsollaWebBrowserAssetManager::UXsollaWebBrowserAssetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultMaterial = FString::Printf(TEXT("/%s/Common/Browser/M_WebTexture.M_WebTexture"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaWebBrowserModule::ModuleName));
	DefaultTranslucentMaterial = FString::Printf(TEXT("/%s/Common/Browser/M_WebTexture_Translucent.M_WebTexture_Translucent"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaWebBrowserModule::ModuleName));

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
