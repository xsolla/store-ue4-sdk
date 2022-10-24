// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowserModule.h"

#include "XsollaWebBrowserAssetManager.h"

#include "IWebBrowserSingleton.h"
#include "Materials/Material.h"
#include "Modules/ModuleManager.h"
#include "WebBrowserModule.h"

const FName FXsollaWebBrowserModule::ModuleName = "XsollaWebBrowser";

void FXsollaWebBrowserModule::StartupModule()
{
	if (WebBrowserAssetMgr == nullptr)
	{
		WebBrowserAssetMgr = NewObject<UXsollaWebBrowserAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
		WebBrowserAssetMgr->LoadDefaultMaterials();

		FWebBrowserInitSettings WebBrowserInitSettings;

		FString ChromeVersion = TEXT("Chrome/87.0.4280.66");

		WebBrowserInitSettings.ProductVersion = FString::Printf(TEXT("%s/%s UnrealEngine/%s Mozilla/5.0 (Linux; Android 10; Redmi Note 7) AppleWebKit/537.36 (KHTML, like Gecko) %s Safari/537.36"), FApp::GetProjectName(), FApp::GetBuildVersion(), *FEngineVersion::Current().ToString(), *ChromeVersion);
		IWebBrowserModule::Get().CustomInitialize(WebBrowserInitSettings);

		IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
		if (WebBrowserSingleton)
		{
			WebBrowserSingleton->SetDefaultMaterial(WebBrowserAssetMgr->GetDefaultMaterial());
			WebBrowserSingleton->SetDefaultTranslucentMaterial(WebBrowserAssetMgr->GetDefaultTranslucentMaterial());
		}
	}
}

void FXsollaWebBrowserModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FXsollaWebBrowserModule, XsollaWebBrowser);
