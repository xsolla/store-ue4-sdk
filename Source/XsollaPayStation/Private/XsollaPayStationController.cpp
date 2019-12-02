// Copyright 2019 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationController.h"

#include "XsollaPayStation.h"
#include "XsollaPayStationDefines.h"
#include "XsollaPayStationSettings.h"

#include "Engine.h"
#include "Modules/ModuleManager.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "FXsollaPayStationModule"

const FString UXsollaPayStationController::PaymentEndpoint(TEXT("https://secure.xsolla.com/paystation3"));
const FString UXsollaPayStationController::SandboxPaymentEndpoint(TEXT("https://sandbox-secure.xsolla.com/paystation3"));

UXsollaPayStationController::UXsollaPayStationController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/Xsolla/Browser/W_PayStationBrowser.W_PayStationBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
}

void UXsollaPayStationController::FetchPaymentToken(const FOnFetchPaymentTokenSuccess& SuccessCallback, const FOnPayStationError& ErrorCallback)
{
	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Settings->TokenRequestURL);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaPayStationController::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPayStationController::LaunchPaymentConsole(const FString& PaymentToken, UUserWidget*& BrowserWidget)
{
	const FString Endpoint = IsSandboxEnabled() ? SandboxPaymentEndpoint : PaymentEndpoint;
	const FString PayStationUrl = FString::Printf(TEXT("%s?access_token=%s"), *Endpoint, *PaymentToken);

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: Loading PayStation: %s"), *VA_FUNC_LINE, *PayStationUrl);

	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();

	// Check for user browser widget override
	auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

	PengindPayStationUrl = PayStationUrl;
	auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
	MyBrowser->AddToViewport(MAX_int32);

	BrowserWidget = MyBrowser;
}

FString UXsollaPayStationController::GetPendingPayStationUrl() const
{
	return PengindPayStationUrl;
}

void UXsollaPayStationController::FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchPaymentTokenSuccess SuccessCallback, FOnPayStationError ErrorCallback)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString PaymentToken = HttpResponse->GetContentAsString();
		UE_LOG(LogXsollaPayStation, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *PaymentToken);

		SuccessCallback.ExecuteIfBound(PaymentToken);
	}
	else
	{
		UE_LOG(LogXsollaPayStation, Warning, TEXT("%s: request failed"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(204, 0, TEXT("No response"));
	}
}

bool UXsollaPayStationController::IsSandboxEnabled() const
{
	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();
	bool bIsSandboxEnabled = Settings->bSandbox;

#if UE_BUILD_SHIPPING
	bIsSandboxEnabled = Settings->bSandbox && Settings->bEnableSandboxInShipping;
	if (bIsSandboxEnabled)
	{
		UE_LOG(LogXsollaPayStation, Warning, TEXT("%s: Sandbox should be disabled in Shipping build"), *VA_FUNC_LINE);
	}
#endif // UE_BUILD_SHIPPING

	return bIsSandboxEnabled;
}

TSharedRef<IHttpRequest> UXsollaPayStationController::CreateHttpRequest(const FString& Url)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=paystation&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_PAYSTATION_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	HttpRequest->SetVerb(TEXT("POST"));

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("PAYSTATION"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_PAYSTATION_VERSION);

	return HttpRequest;
}

#undef LOCTEXT_NAMESPACE