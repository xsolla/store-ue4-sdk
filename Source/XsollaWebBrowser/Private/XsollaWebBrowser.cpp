// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaWebBrowser.h"

#include "XsollaWebBrowserDefines.h"
#include "Async/TaskGraphInterfaces.h"
#include "Blueprint/UserWidget.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "SWebBrowser.h"
#include "WebBrowserModule.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "XsollaUtilsLoggingHelper.h"

#define LOCTEXT_NAMESPACE "XsollaWebBrowser"

namespace
{
	enum class EXsollaPopupSchemeType : uint8
	{
		Empty,
		Http,
		Https,
		About,
		Javascript,
		Other
	};

	EXsollaPopupSchemeType ClassifyPopupScheme(const FString& Url, FString& OutScheme)
	{
		const FString TrimmedUrl = Url.TrimStartAndEnd();
		if (TrimmedUrl.IsEmpty())
		{
			OutScheme = TEXT("empty");
			return EXsollaPopupSchemeType::Empty;
		}

		int32 SchemeSeparatorIdx = INDEX_NONE;
		if (!TrimmedUrl.FindChar(TEXT(':'), SchemeSeparatorIdx) || SchemeSeparatorIdx <= 0)
		{
			OutScheme = TEXT("none");
			return EXsollaPopupSchemeType::Other;
		}

		OutScheme = TrimmedUrl.Left(SchemeSeparatorIdx).ToLower();
		if (OutScheme == TEXT("http"))
		{
			return EXsollaPopupSchemeType::Http;
		}
		if (OutScheme == TEXT("https"))
		{
			return EXsollaPopupSchemeType::Https;
		}
		if (OutScheme == TEXT("about"))
		{
			return EXsollaPopupSchemeType::About;
		}
		if (OutScheme == TEXT("javascript"))
		{
			return EXsollaPopupSchemeType::Javascript;
		}

		return EXsollaPopupSchemeType::Other;
	}

	const TCHAR* PopupSchemeTypeToString(const EXsollaPopupSchemeType SchemeType)
	{
		switch (SchemeType)
		{
		case EXsollaPopupSchemeType::Empty:
			return TEXT("Empty");
		case EXsollaPopupSchemeType::Http:
			return TEXT("Http");
		case EXsollaPopupSchemeType::Https:
			return TEXT("Https");
		case EXsollaPopupSchemeType::About:
			return TEXT("About");
		case EXsollaPopupSchemeType::Javascript:
			return TEXT("Javascript");
		default:
			return TEXT("Other");
		}
	}

	bool ShouldRoutePopupToExternalHandler(const EXsollaPopupSchemeType SchemeType)
	{
		return SchemeType == EXsollaPopupSchemeType::Http || SchemeType == EXsollaPopupSchemeType::Https;
	}

	bool IsExternalPaystationPopupFrame(const FString& FrameName)
	{
		return FrameName.StartsWith(TEXT("PayStationExternalPayment"), ESearchCase::IgnoreCase);
	}

	void NotifyAndCloseOwningUserWidget(UXsollaWebBrowser* WebBrowser)
	{
		if (!IsValid(WebBrowser))
		{
			return;
		}

		if (UUserWidget* OwnerUserWidget = WebBrowser->GetTypedOuter<UUserWidget>())
		{
			static const FName ExecuteBrowserClosedName(TEXT("ExecuteBrowserClosed"));
			if (UFunction* ExecuteBrowserClosedFunction = OwnerUserWidget->FindFunction(ExecuteBrowserClosedName))
			{
				// Store browser wrapper uses ExecuteBrowserClosed(bool). Other wrappers may have different signatures.
				if (ExecuteBrowserClosedFunction->NumParms == 1)
				{
					struct FExecuteBrowserClosedParams
					{
						bool bIsManually;
					};

					FExecuteBrowserClosedParams Params{ false };
					OwnerUserWidget->ProcessEvent(ExecuteBrowserClosedFunction, &Params);
				}
			}

			OwnerUserWidget->RemoveFromParent();
		}
	}
}

UXsollaWebBrowser::UXsollaWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UXsollaWebBrowser::LoadURL(FString NewURL)
{
	FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(NewURL);
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Loading URL: %s"), *VA_FUNC_LINE, *SanitizedUrl);

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
	FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(DummyURL);
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Loading HTML with dummy URL: %s"), *VA_FUNC_LINE, *SanitizedUrl);

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
		FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(CurrentUrl);
		UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Current URL: %s"), *VA_FUNC_LINE, *SanitizedUrl);
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
	FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(InitialURL);
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Creating web browser widget with initial URL: %s"),
		*VA_FUNC_LINE, *SanitizedUrl);

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
	FString Url = InText.ToString();
	FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(Url);
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: URL changed to: %s"), *VA_FUNC_LINE, *SanitizedUrl);
	OnUrlChanged.Broadcast(InText);
}

void UXsollaWebBrowser::HandleOnPageLoaded()
{
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: Page loaded"), *VA_FUNC_LINE);
	OnPageLoaded.Broadcast();
}

bool UXsollaWebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	FString SanitizedUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(URL);
	FString Scheme;
	const EXsollaPopupSchemeType SchemeType = ClassifyPopupScheme(URL, Scheme);
	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE before popup - URL: %s, Frame: %s, Scheme: %s, SchemeType: %s"),
		*VA_FUNC_LINE, *SanitizedUrl, *Frame, *Scheme, PopupSchemeTypeToString(SchemeType));

	if (!ShouldRoutePopupToExternalHandler(SchemeType))
	{
		// Paystation can bootstrap external payment windows with about:blank.
		// In embedded mode this popup does not progress, so route to external browser via existing delegate using current URL.
		if (OnBeforePopup.IsBound() && IsExternalPaystationPopupFrame(Frame) && WebBrowserWidget.IsValid())
		{
			const FString FallbackUrl = WebBrowserWidget->GetUrl();
			const bool bHasHttpFallbackUrl = FallbackUrl.StartsWith(TEXT("http://"), ESearchCase::IgnoreCase)
				|| FallbackUrl.StartsWith(TEXT("https://"), ESearchCase::IgnoreCase);
			if (bHasHttpFallbackUrl)
			{
				const FString SanitizedFallbackUrl = XsollaUtilsLoggingHelper::SanitizeUrlForLogging(FallbackUrl);
				UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE decision=external_fallback_current_paystation_url frame=%s fallback_url=%s"),
					*VA_FUNC_LINE, *Frame, *SanitizedFallbackUrl);

				if (IsInGameThread())
				{
					OnBeforePopup.Broadcast(FallbackUrl, Frame);
					NotifyAndCloseOwningUserWidget(this);
					UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE embedded_browser_closed_after_external_fallback"), *VA_FUNC_LINE);
				}
				else
				{
					TWeakObjectPtr<UXsollaWebBrowser> WeakThis = this;
					FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, FallbackUrl, Frame]()
					{
						if (WeakThis.IsValid() && WeakThis->OnBeforePopup.IsBound())
						{
							WeakThis->OnBeforePopup.Broadcast(FallbackUrl, Frame);
							NotifyAndCloseOwningUserWidget(WeakThis.Get());
							UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE embedded_browser_closed_after_external_fallback"), *VA_FUNC_LINE);
						}
					}, TStatId(), nullptr, ENamedThreads::GameThread);
				}

				return true;
			}
		}

		UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE decision=allow_embedded_popup (non-http(s) scheme)"), *VA_FUNC_LINE);
		return false;
	}

	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE decision=external_handler thread=game"), *VA_FUNC_LINE);
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE decision=dispatch_to_game_thread"), *VA_FUNC_LINE);
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

	UE_LOG(LogXsollaWebBrowser, Log, TEXT("%s: XS_POPUP_TRACE no external delegate bound, fallback=embedded"), *VA_FUNC_LINE);
	return false;
}

#if WITH_EDITOR
const FText UXsollaWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Xsolla", "Xsolla");
}
#endif

#undef LOCTEXT_NAMESPACE
