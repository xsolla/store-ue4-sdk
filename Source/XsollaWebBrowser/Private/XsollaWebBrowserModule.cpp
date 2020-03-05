// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaWebBrowserModule.h"

#include "XsollaWebBrowserAssetManager.h"

#include "IWebBrowserSingleton.h"
#include "Materials/Material.h"
#include "Modules/ModuleManager.h"
#include "WebBrowserModule.h"

class FXsollaWebBrowserModule : public IXsollaWebBrowserModule
{
public:
	virtual void StartupModule() override
	{
		if (WebBrowserAssetMgr == nullptr)
		{
			WebBrowserAssetMgr = NewObject<UXsollaWebBrowserAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
			WebBrowserAssetMgr->LoadDefaultMaterials();

			IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
			if (WebBrowserSingleton)
			{
				WebBrowserSingleton->SetDefaultMaterial(WebBrowserAssetMgr->GetDefaultMaterial());
				WebBrowserSingleton->SetDefaultTranslucentMaterial(WebBrowserAssetMgr->GetDefaultTranslucentMaterial());
			}
		}
	}

	virtual void ShutdownModule() override
	{
	}

private:
	UXsollaWebBrowserAssetManager* WebBrowserAssetMgr;
};

IMPLEMENT_MODULE(FXsollaWebBrowserModule, XsollaWebBrowser);
