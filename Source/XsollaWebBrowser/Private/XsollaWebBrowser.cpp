// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowser.h"

#include "XsollaWebBrowserDefines.h"
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
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Loading URL: %s"), *VA_FUNC_LINE, *NewURL);

	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->LoadURL(NewURL);
	}
	else
	{
		UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserWidget is not valid"), *VA_FUNC_LINE);
	}
}

void UXsollaWebBrowser::LoadHtml(FString Contents, FString DummyURL)
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Loading HTML with dummy URL: %s"), *VA_FUNC_LINE, *DummyURL);

	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->LoadString(Contents, DummyURL);
	}
	else
	{
		UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserWidget is not valid"), *VA_FUNC_LINE);
	}
}

void UXsollaWebBrowser::ExecuteJavascript(FString ScriptText)
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Executing JavaScript"), *VA_FUNC_LINE);

	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->ExecuteJavascript(ScriptText);
	}
	else
	{
		UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserWidget is not valid"), *VA_FUNC_LINE);
	}
}

FString UXsollaWebBrowser::GetUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		FString CurrentUrl = WebBrowserWidget->GetUrl();
		UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Current URL: %s"), *VA_FUNC_LINE, *CurrentUrl);
		return CurrentUrl;
	}

	UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserWidget is not valid, returning empty URL"), *VA_FUNC_LINE);
	return FString();
}

void UXsollaWebBrowser::ClearCache() const
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Clearing browser cache"), *VA_FUNC_LINE);

	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	if (WebBrowserSingleton)
	{
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
		if (CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Browser cookies deleted"), *VA_FUNC_LINE);
		}
		else
		{
			UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: CookieManager is not valid"), *VA_FUNC_LINE);
		}
	}
	else
	{
		UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserSingleton is not valid"), *VA_FUNC_LINE);
	}
}

void UXsollaWebBrowser::GoBack() const
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Attempting to navigate back"), *VA_FUNC_LINE);

	if (WebBrowserWidget.IsValid())
	{
		if (WebBrowserWidget->CanGoBack())
		{
			WebBrowserWidget->GoBack();
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Navigated back"), *VA_FUNC_LINE);
		}
		else
		{
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Cannot go back - no history available"), *VA_FUNC_LINE);
		}
	}
	else
	{
		UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserWidget is not valid"), *VA_FUNC_LINE);
	}
}

void UXsollaWebBrowser::GoForward() const
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Attempting to navigate forward"), *VA_FUNC_LINE);

	if (WebBrowserWidget.IsValid())
	{
		if (WebBrowserWidget->CanGoForward())
		{
			WebBrowserWidget->GoForward();
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Navigated forward"), *VA_FUNC_LINE);
		}
		else
		{
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Cannot go forward - no forward history available"), *VA_FUNC_LINE);
		}
	}
	else
	{
		UE_LOG(LogXsollaWebBrowser, Warning, TEXT("%s: WebBrowserWidget is not valid"), *VA_FUNC_LINE);
	}
}

void UXsollaWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Releasing slate resources"), *VA_FUNC_LINE);

	Super::ReleaseSlateResources(bReleaseChildren);

	WebBrowserWidget.Reset();

	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: WebBrowserWidget reset"), *VA_FUNC_LINE);
}

TSharedRef<SWidget> UXsollaWebBrowser::RebuildWidget()
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Rebuilding widget"), *VA_FUNC_LINE);

	// clang-format off
	if (IsDesignTime())
	{
		UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Creating design time widget"), *VA_FUNC_LINE);
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
		UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Creating web browser widget with initial URL: %s"),
			*VA_FUNC_LINE, *InitialURL);

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
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: URL changed to: %s"), *VA_FUNC_LINE, *InText.ToString());
	OnUrlChanged.Broadcast(InText);
}

void UXsollaWebBrowser::HandleOnPageLoaded()
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Page loaded"), *VA_FUNC_LINE);
	OnPageLoaded.Broadcast();
}

bool UXsollaWebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Before popup event - URL: %s, Frame: %s"),
		*VA_FUNC_LINE, *URL, *Frame);

	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Broadcasting OnBeforePopup event on game thread"), *VA_FUNC_LINE);
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Dispatching OnBeforePopup event to game thread"), *VA_FUNC_LINE);
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

	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: No OnBeforePopup delegate bound"), *VA_FUNC_LINE);
	return false;
}

#if WITH_EDITOR
const FText UXsollaWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Xsolla", "Xsolla");
}
#endif

#undef LOCTEXT_NAMESPACE
