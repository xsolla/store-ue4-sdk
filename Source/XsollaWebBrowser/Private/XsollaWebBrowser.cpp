// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowser.h"

#include "Async/TaskGraphInterfaces.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "SWebBrowser.h"
#include "WebBrowserModule.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "XsollaWebBrowser"

UXsollaWebBrowser::UXsollaWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UXsollaWebBrowser::LoadURL(FString NewURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadURL(NewURL);
	}
}

void UXsollaWebBrowser::LoadHtml(FString Contents, FString DummyURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadString(Contents, DummyURL);
	}
}

void UXsollaWebBrowser::ExecuteJavascript(FString ScriptText)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->ExecuteJavascript(ScriptText);
	}
}

FString UXsollaWebBrowser::GetUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetUrl();
	}

	return FString();
}

void UXsollaWebBrowser::ClearCache() const
{
	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	if (WebBrowserSingleton)
	{
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
		if (CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
		}
	}
}

void UXsollaWebBrowser::GoBack() const
{
	if (WebBrowserWidget.IsValid() && WebBrowserWidget->CanGoBack())
	{
		return WebBrowserWidget->GoBack();
	}
}

void UXsollaWebBrowser::GoForward() const
{
	if (WebBrowserWidget.IsValid() && WebBrowserWidget->CanGoForward())
	{
		return WebBrowserWidget->GoForward();
	}
}

void UXsollaWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	WebBrowserWidget.Reset();
}

TSharedRef<SWidget> UXsollaWebBrowser::RebuildWidget()
{
	// clang-format off
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Xsolla Web Browser", "Xsolla Web Browser"))
			];
	}
	else
	{
		WebBrowserWidget = SNew(SWebBrowser)
			.InitialURL(InitialURL)
			.ShowControls(false)
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
			.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnPageLoaded));

		return WebBrowserWidget.ToSharedRef();
	}
	// clang-format on
}

void UXsollaWebBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

void UXsollaWebBrowser::HandleOnPageLoaded()
{
	OnPageLoaded.Broadcast();
}

bool UXsollaWebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// clang-format off
			// Retry on the GameThread now
			TWeakObjectPtr<UXsollaWebBrowser> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->HandleOnBeforePopup(URL, Frame);
				}
			}, TStatId(), nullptr, ENamedThreads::GameThread);
			// clang-format on
		}

		return true;
	}

	return false;
}

#if WITH_EDITOR
const FText UXsollaWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Xsolla", "Xsolla");
}
#endif

#undef LOCTEXT_NAMESPACE
