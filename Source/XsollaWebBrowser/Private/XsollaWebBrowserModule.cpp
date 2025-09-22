// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowserModule.h"

#include "XsollaWebBrowserAssetManager.h"
#include "XsollaWebBrowserDefines.h"

#include "IWebBrowserSingleton.h"
#include "Materials/Material.h"
#include "Modules/ModuleManager.h"
#include "WebBrowserModule.h"

DEFINE_LOG_CATEGORY(LogXsollaWebBrowser);

const FName FXsollaWebBrowserModule::ModuleName = "XsollaWebBrowser";

void FXsollaWebBrowserModule::StartupModule()
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XsollaWebBrowser module starting up"), *VA_FUNC_LINE);

	if (WebBrowserAssetMgr == nullptr)
	{
		WebBrowserAssetMgr = NewObject<UXsollaWebBrowserAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
		WebBrowserAssetMgr->LoadDefaultMaterials();
		UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Default materials loaded"), *VA_FUNC_LINE);

		IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
		if (WebBrowserSingleton)
		{
			WebBrowserSingleton->SetDefaultMaterial(WebBrowserAssetMgr->GetDefaultMaterial());
			WebBrowserSingleton->SetDefaultTranslucentMaterial(WebBrowserAssetMgr->GetDefaultTranslucentMaterial());
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Default materials set on WebBrowserSingleton"), *VA_FUNC_LINE);
		}
		else
		{
			UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: Failed to get WebBrowserSingleton"), *VA_FUNC_LINE);
		}
	}

	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XsollaWebBrowser module initialized successfully"), *VA_FUNC_LINE);
}

void FXsollaWebBrowserModule::ShutdownModule()
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XsollaWebBrowser module shutting down"), *VA_FUNC_LINE);
}

IMPLEMENT_MODULE(FXsollaWebBrowserModule, XsollaWebBrowser);
