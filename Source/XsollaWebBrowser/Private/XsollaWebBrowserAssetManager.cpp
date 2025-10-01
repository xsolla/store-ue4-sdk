// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowserAssetManager.h"

#include "XsollaUtilsLibrary.h"
#include "XsollaWebBrowserDefines.h"
#include "XsollaWebBrowserModule.h"

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
#include "WebBrowserTexture.h"
#endif

UXsollaWebBrowserAssetManager::UXsollaWebBrowserAssetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Initializing XsollaWebBrowserAssetManager"), *VA_FUNC_LINE);

	DefaultMaterial = FString::Printf(TEXT("/%s/Browser/Assets/M_WebTexture.M_WebTexture"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaWebBrowserModule::ModuleName));
	DefaultTranslucentMaterial = FString::Printf(TEXT("/%s/Browser/Assets/M_WebTexture_Translucent.M_WebTexture_Translucent"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaWebBrowserModule::ModuleName));

	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Default material path: %s"), *VA_FUNC_LINE, *DefaultMaterial.ToString());
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Default translucent material path: %s"), *VA_FUNC_LINE, *DefaultTranslucentMaterial.ToString());

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
	// Add a hard reference to UXsollaWebBrowserTexture, without this the WebBrowserTexture DLL never gets loaded on Windows.
	UWebBrowserTexture::StaticClass();
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Added hard reference to WebBrowserTexture"), *VA_FUNC_LINE);
#endif
};

void UXsollaWebBrowserAssetManager::LoadDefaultMaterials()
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Loading default materials"), *VA_FUNC_LINE);

	DefaultMaterial.LoadSynchronous();
	DefaultTranslucentMaterial.LoadSynchronous();

	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Default materials loaded successfully"), *VA_FUNC_LINE);
}

UMaterial* UXsollaWebBrowserAssetManager::GetDefaultMaterial() const
{
	UMaterial* Material = DefaultMaterial.Get();
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Getting default material: %s"),
		*VA_FUNC_LINE, Material ? *Material->GetName() : TEXT("NULL"));
	return Material;
}

UMaterial* UXsollaWebBrowserAssetManager::GetDefaultTranslucentMaterial() const
{
	UMaterial* Material = DefaultTranslucentMaterial.Get();
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Getting default translucent material: %s"),
		*VA_FUNC_LINE, Material ? *Material->GetName() : TEXT("NULL"));
	return Material;
}
