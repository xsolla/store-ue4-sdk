// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreSubsystem.h"

#include "XsollaStore.h"
#include "XsollaStoreCurrencyFormat.h"
#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreImageLoader.h"
#include "XsollaStoreSave.h"
#include "XsollaStoreSettings.h"

#include "Dom/JsonObject.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "JsonObjectConverter.h"
#include "Kismet/KismetTextLibrary.h"
#include "Misc/Base64.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

UXsollaStoreSubsystem::UXsollaStoreSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CurrencyLibraryObj(TEXT("DataTable'/Xsolla/Data/currency-format.currency-format'"));
	CurrencyLibrary = CurrencyLibraryObj.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/Xsolla/Browser/W_StoreBrowser.W_StoreBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;

	// @TODO https://github.com/xsolla/store-ue4-sdk/issues/68
	CachedCartCurrency = TEXT("USD");
}

void UXsollaStoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaStoreSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaStoreSubsystem::Initialize(const FString& InProjectId)
{
	ProjectId = InProjectId;

	LoadData();

	// Check image loader is exsits, because initialization can be called multiple times
	if (!ImageLoader)
	{
		ImageLoader = NewObject<UXsollaStoreImageLoader>();
	}
}

void UXsollaStoreSubsystem::UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_items"), *ProjectId);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateVirtualItems_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateItemGroups(const FString& Locale, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/groups?locale=%s"), *ProjectId, *UsedLocale);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateItemGroups_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateInventory(const FString& AuthToken, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/inventory/items"), *ProjectId);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateInventory_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencies(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency"), *ProjectId);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateVirtualCurrencies_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/package"), *ProjectId);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyBalance(const FString& AuthToken, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/virtual_currency_balance"), *ProjectId);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateVirtualCurrencyBalance_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU, const FString& Currency, const FString& Country, const FString& Locale, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	if (!Currency.IsEmpty())
		RequestDataJson->SetStringField(TEXT("currency"), Currency);
	if (!Country.IsEmpty())
		RequestDataJson->SetStringField(TEXT("country"), Country);
	if (!Locale.IsEmpty())
		RequestDataJson->SetStringField(TEXT("locale"), Locale);

	RequestDataJson->SetBoolField(TEXT("sandbox"), IsSandboxEnabled());

	FString theme;

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
	switch (Settings->PaymentInterfaceTheme)
	{
	case EXsollaPaymentUiTheme::Default:
		theme = TEXT("default");
		break;

	case EXsollaPaymentUiTheme::DefaultDark:
		theme = TEXT("default_dark");
		break;

	case EXsollaPaymentUiTheme::Dark:
		theme = TEXT("dark");
		break;

	default:
		theme = TEXT("dark");
	}

	TSharedPtr<FJsonObject> PaymentUiSettingsJson = MakeShareable(new FJsonObject);
	PaymentUiSettingsJson->SetStringField(TEXT("theme"), theme);

	TSharedPtr<FJsonObject> PaymentSettingsJson = MakeShareable(new FJsonObject);
	PaymentSettingsJson->SetObjectField(TEXT("ui"), PaymentUiSettingsJson);

	RequestDataJson->SetObjectField(TEXT("settings"), PaymentSettingsJson);

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/item/%s"), *ProjectId, *ItemSKU);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));

	if (Settings->bBuildForSteam)
	{
		TSharedPtr<FJsonObject> PayloadJsonObject;
		if (!ParseTokenPayload(AuthToken, PayloadJsonObject))
		{
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
			ErrorCallback.ExecuteIfBound(0, 0, TEXT("Can't parse token payload"));
			return;
		}

		FString SteamIdUrl;
		if (!PayloadJsonObject->TryGetStringField(TEXT("id"), SteamIdUrl))
		{
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't find Steam profile ID in token payload"), *VA_FUNC_LINE);
			ErrorCallback.ExecuteIfBound(0, 0, TEXT("Can't find Steam profile ID in token payload"));
			return;
		}

		// Extract ID value from user's Steam profile URL
		FString SteamId;
		int SteamIdIndex;
		if (SteamIdUrl.FindLastChar('/', SteamIdIndex))
		{
			SteamId = SteamIdUrl.RightChop(SteamIdIndex + 1);
		}

		HttpRequest->SetHeader(TEXT("x-steam-userid"), SteamId);
	}

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::FetchCartPaymentToken(const FString& AuthToken, const FString& CartId, const FString& Currency, const FString& Country, const FString& Locale, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	if (!Currency.IsEmpty())
		RequestDataJson->SetStringField(TEXT("currency"), Currency);
	if (!Country.IsEmpty())
		RequestDataJson->SetStringField(TEXT("country"), Country);
	if (!Locale.IsEmpty())
		RequestDataJson->SetStringField(TEXT("locale"), Locale);

	RequestDataJson->SetBoolField(TEXT("sandbox"), IsSandboxEnabled());

	FString theme;

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
	switch (Settings->PaymentInterfaceTheme)
	{
	case EXsollaPaymentUiTheme::Default:
		theme = TEXT("default");
		break;

	case EXsollaPaymentUiTheme::DefaultDark:
		theme = TEXT("default_dark");
		break;

	case EXsollaPaymentUiTheme::Dark:
		theme = TEXT("dark");
		break;

	default:
		theme = TEXT("dark");
	}

	TSharedPtr<FJsonObject> PaymentUiSettingsJson = MakeShareable(new FJsonObject);
	PaymentUiSettingsJson->SetStringField(TEXT("theme"), theme);

	TSharedPtr<FJsonObject> PaymentSettingsJson = MakeShareable(new FJsonObject);
	PaymentSettingsJson->SetObjectField(TEXT("ui"), PaymentUiSettingsJson);

	RequestDataJson->SetObjectField(TEXT("settings"), PaymentSettingsJson);

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/cart"), *ProjectId);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/cart/%s"), *ProjectId, *Cart.cart_id);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));

	if (Settings->bBuildForSteam)
	{
		TSharedPtr<FJsonObject> PayloadJsonObject;
		if (!ParseTokenPayload(AuthToken, PayloadJsonObject))
		{
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
			ErrorCallback.ExecuteIfBound(0, 0, TEXT("Can't parse token payload"));
			return;
		}

		FString SteamIdUrl;
		if (!PayloadJsonObject->TryGetStringField(TEXT("id"), SteamIdUrl))
		{
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't find Steam profile ID in token payload"), *VA_FUNC_LINE);
			ErrorCallback.ExecuteIfBound(0, 0, TEXT("Can't find Steam profile ID in token payload"));
			return;
		}

		// Extract ID value from user's Steam profile URL
		FString SteamId;
		int SteamIdIndex;
		if (SteamIdUrl.FindLastChar('/', SteamIdIndex))
		{
			SteamId = SteamIdUrl.RightChop(SteamIdIndex + 1);
		}

		HttpRequest->SetHeader(TEXT("x-steam-userid"), SteamId);
	}

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::LaunchPaymentConsole(const FString& AccessToken, UUserWidget*& BrowserWidget)
{
	FString PaystationUrl;
	if (IsSandboxEnabled())
	{
		PaystationUrl = FString::Printf(TEXT("https://sandbox-secure.xsolla.com/paystation3?access_token=%s"), *AccessToken);
	}
	else
	{
		PaystationUrl = FString::Printf(TEXT("https://secure.xsolla.com/paystation3?access_token=%s"), *AccessToken);
	}

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
	if (Settings->bUsePlatformBrowser)
	{
		UE_LOG(LogXsollaStore, Log, TEXT("%s: Launching Paystation: %s"), *VA_FUNC_LINE, *PaystationUrl);

		BrowserWidget = nullptr;

		FPlatformProcess::LaunchURL(*PaystationUrl, nullptr, nullptr);
	}
	else
	{
		UE_LOG(LogXsollaStore, Log, TEXT("%s: Loading Paystation: %s"), *VA_FUNC_LINE, *PaystationUrl);

		// Check for user browser widget override
		auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

		PengindPaystationUrl = PaystationUrl;
		auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
		MyBrowser->AddToViewport(MAX_int32);

		BrowserWidget = MyBrowser;
	}
}

void UXsollaStoreSubsystem::CheckOrder(const FString& AuthToken, int32 OrderId, const FOnCheckOrder& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/order/%d"), *ProjectId, OrderId);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::CheckOrder_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::CreateCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/project/%s/cart"), *ProjectId);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::CreateCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::ClearCart(const FString& AuthToken, const FString& CartId, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/clear"), *ProjectId);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/clear"), *ProjectId, *Cart.cart_id);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::PUT, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::ClearCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();

	// Just cleanup local cart
	Cart.Items.Empty();
	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreSubsystem::UpdateCart(const FString& AuthToken, const FString& CartId, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart"), *ProjectId);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s"), *ProjectId, *Cart.cart_id);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::UpdateCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::AddToCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, int32 Quantity, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetNumberField(TEXT("quantity"), Quantity);

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/item/%s"), *ProjectId, *ItemSKU);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/item/%s"), *ProjectId, *Cart.cart_id, *ItemSKU);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::PUT, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::AddToCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();

	// Try to update item quantity
	auto CartItem = Cart.Items.FindByPredicate([ItemSKU](const FStoreCartItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	if (CartItem)
	{
		CartItem->quantity = FMath::Max(0, Quantity);
	}
	else
	{
		auto StoreItem = ItemsData.Items.FindByPredicate([ItemSKU](const FStoreItem& InItem) {
			return InItem.sku == ItemSKU;
		});

		if (StoreItem)
		{
			FStoreCartItem Item(*StoreItem);
			Item.quantity = FMath::Max(0, Quantity);

			// @TODO Predict price locally before cart sync https://github.com/xsolla/store-ue4-sdk/issues/68

			Cart.Items.Add(Item);
		}
		else
		{
			auto CurrencyPackageItem = VirtualCurrencyPackages.Items.FindByPredicate([ItemSKU](const FVirtualCurrencyPackage& InItem) {
				return InItem.sku == ItemSKU;
			});

			if (CurrencyPackageItem)
			{
				FStoreCartItem Item(*CurrencyPackageItem);
				Item.quantity = FMath::Max(0, Quantity);

				Cart.Items.Add(Item);
			}
			else
			{
				UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't find provided SKU in local cache: %s"), *VA_FUNC_LINE, *ItemSKU);
			}
		}
	}

	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreSubsystem::RemoveFromCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/item/%s"), *ProjectId, *ItemSKU);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/item/%s"), *ProjectId, *Cart.cart_id, *ItemSKU);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::DELETE, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::RemoveFromCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();

	for (int32 i = Cart.Items.Num() - 1; i >= 0; --i)
	{
		if (Cart.Items[i].sku == ItemSKU)
		{
			Cart.Items.RemoveAt(i);
			break;
		}
	}

	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreSubsystem::ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU, int32 Quantity, const FString& InstanceID, const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("sku"), ItemSKU);

	if (Quantity == 0)
	{
		RequestDataJson->SetObjectField(TEXT("quantity"), nullptr);
	}
	else
	{
		RequestDataJson->SetNumberField(TEXT("quantity"), Quantity);
	}

	if (InstanceID.IsEmpty())
	{
		RequestDataJson->SetObjectField(TEXT("instance_id"), nullptr);
	}
	else
	{
		RequestDataJson->SetStringField(TEXT("instance_id"), InstanceID);
	}

	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/inventory/item/consume"), *ProjectId);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::ConsumeInventoryItem_HttpRequestComplete, SuccessCallback, ErrorCallback);

	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetVirtualCurrency(const FString& CurrencySKU, const FOnCurrencyUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/sku/%s"), *ProjectId, *CurrencySKU);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::GetVirtualCurrency_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetVirtualCurrencyPackage(const FString& PackageSKU, const FOnCurrencyPackageUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/package/sku/%s"), *ProjectId, *PackageSKU);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::GetVirtualCurrencyPackage_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::BuyItemWithVirtualCurrency(const FString& AuthToken, const FString& ItemSKU, const FString& CurrencySKU, const FOnPurchaseUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/item/%s/virtual/%s"), *ProjectId, *ItemSKU, *CurrencySKU);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreSubsystem::BuyItemWithVirtualCurrency_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreItemsData::StaticStruct(), &ItemsData))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	// Update categories now
	for (auto& Item : ItemsData.Items)
	{
		for (auto& ItemGroup : Item.groups)
		{
			ItemsData.GroupIds.Add(ItemGroup.external_id);
		}
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::UpdateItemGroups_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	// Deserialize to new object
	FStoreItemsData GroupsData;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreItemsData::StaticStruct(), &GroupsData))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	// Cache data as it should now
	ItemsData.Groups = GroupsData.Groups;

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::UpdateInventory_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FStoreInventory newInventory;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreInventory::StaticStruct(), &newInventory))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	Inventory = newInventory;

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencies_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FVirtualCurrencyData::StaticStruct(), &VirtualCurrencyData))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FVirtualCurrencyPackagesData::StaticStruct(), &VirtualCurrencyPackages))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FVirtualCurrencyBalanceData::StaticStruct(), &VirtualCurrencyBalance))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FString AccessToken = JsonObject->GetStringField(TEXT("token"));
	int32 OrderId = JsonObject->GetNumberField(TEXT("order_id"));

	SuccessCallback.ExecuteIfBound(AccessToken, OrderId);
}

void UXsollaStoreSubsystem::CheckOrder_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckOrder SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	int32 OrderId = JsonObject->GetNumberField(TEXT("order_id"));
	FString Status = JsonObject->GetStringField(TEXT("status"));
	EXsollaOrderStatus OrderStatus = EXsollaOrderStatus::Unknown;

	if (Status == TEXT("new"))
	{
		OrderStatus = EXsollaOrderStatus::New;
	}
	else if (Status == TEXT("paid"))
	{
		OrderStatus = EXsollaOrderStatus::Paid;
	}
	else if (Status == TEXT("done"))
	{
		OrderStatus = EXsollaOrderStatus::Done;
	}
	else
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: Unknown order status: %s [%d]"), *VA_FUNC_LINE, *Status, OrderId);
	}

	SuccessCallback.ExecuteIfBound(OrderId, OrderStatus);
}

void UXsollaStoreSubsystem::CreateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		ProcessNextCartRequest();
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	Cart = FStoreCart(JsonObject->GetStringField(TEXT("cart_id")));
	OnCartUpdate.Broadcast(Cart);

	SaveData();

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::ClearCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		ProcessNextCartRequest();
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();

	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::UpdateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		ProcessNextCartRequest();
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreCart::StaticStruct(), &Cart))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	OnCartUpdate.Broadcast(Cart);

	SuccessCallback.ExecuteIfBound();

	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::AddToCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		UpdateCart(CachedAuthToken, CachedCartId, SuccessCallback, ErrorCallback);
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();

	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::RemoveFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		UpdateCart(CachedAuthToken, CachedCartId, SuccessCallback, ErrorCallback);
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();

	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::ConsumeInventoryItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::GetVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCurrencyUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		ProcessNextCartRequest();
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FVirtualCurrency currency;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FVirtualCurrency::StaticStruct(), &currency))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	SuccessCallback.ExecuteIfBound(currency);
}

void UXsollaStoreSubsystem::GetVirtualCurrencyPackage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCurrencyPackageUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		ProcessNextCartRequest();
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FVirtualCurrencyPackage currencyPackage;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FVirtualCurrencyPackage::StaticStruct(), &currencyPackage))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	SuccessCallback.ExecuteIfBound(currencyPackage);
}

void UXsollaStoreSubsystem::BuyItemWithVirtualCurrency_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	int32 OrderId = JsonObject->GetNumberField(TEXT("order_id"));

	SuccessCallback.ExecuteIfBound(OrderId);
}

bool UXsollaStoreSubsystem::HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreError ErrorCallback)
{
	FString ErrorStr;
	int32 ErrorCode = 0;
	int32 StatusCode = 204;
	FString ResponseStr = TEXT("invalid");

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();

		if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			StatusCode = HttpResponse->GetResponseCode();
			ErrorStr = FString::Printf(TEXT("Invalid response. code=%d error=%s"), HttpResponse->GetResponseCode(), *ResponseStr);

			// Example: {"statusCode":403,"errorCode":0,"errorMessage":"Token not found"}
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				static const FString ErrorFieldName = TEXT("errorMessage");
				if (JsonObject->HasTypedField<EJson::String>(ErrorFieldName))
				{
					StatusCode = JsonObject->GetNumberField(TEXT("statusCode"));
					ErrorCode = JsonObject->GetNumberField(TEXT("errorCode"));
					ErrorStr = JsonObject->GetStringField(ErrorFieldName);
				}
				else
				{
					ErrorStr = FString::Printf(TEXT("Can't deserialize error json: no field '%s' found"), *ErrorFieldName);
				}
			}
			else
			{
				ErrorStr = TEXT("Can't deserialize error json");
			}
		}
	}
	else
	{
		ErrorStr = TEXT("No response");
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: request failed (%s): %s"), *VA_FUNC_LINE, *ErrorStr, *ResponseStr);
		ErrorCallback.ExecuteIfBound(StatusCode, ErrorCode, ErrorStr);
		return true;
	}

	return false;
}

void UXsollaStoreSubsystem::LoadData()
{
	auto CartData = UXsollaStoreSave::Load();

	CachedCartCurrency = CartData.CartCurrency;
	Cart.cart_id = CartData.CartId;

	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreSubsystem::SaveData()
{
	UXsollaStoreSave::Save(FXsollaStoreSaveData(Cart.cart_id, CachedCartCurrency));
}

bool UXsollaStoreSubsystem::IsSandboxEnabled() const
{
	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
	bool bIsSandboxEnabled = Settings->bSandbox;

#if UE_BUILD_SHIPPING
	bIsSandboxEnabled = Settings->bSandbox && Settings->bEnableSandboxInShipping;
	if (bIsSandboxEnabled)
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: Sandbox should be disabled in Shipping build"), *VA_FUNC_LINE);
	}
#endif // UE_BUILD_SHIPPING

	return bIsSandboxEnabled;
}

TSharedRef<IHttpRequest> UXsollaStoreSubsystem::CreateHttpRequest(const FString& Url, const EXsollaRequestVerb Verb, const FString& AuthToken, const FString& Content)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	// Temporal solution with headers processing on server-side #37
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=store&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_STORE_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("STORE"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_STORE_VERSION);

	switch (Verb)
	{
	case EXsollaRequestVerb::GET:
		HttpRequest->SetVerb(TEXT("GET"));

		// Check that we doen't provide content with GET request
		if (!Content.IsEmpty())
		{
			UE_LOG(LogXsollaStore, Warning, TEXT("%s: Request content is not empty for GET request. Maybe you should use POST one?"), *VA_FUNC_LINE);
		}
		break;

	case EXsollaRequestVerb::POST:
		HttpRequest->SetVerb(TEXT("POST"));
		break;

	case EXsollaRequestVerb::PUT:
		HttpRequest->SetVerb(TEXT("PUT"));
		break;

	case EXsollaRequestVerb::DELETE:
		HttpRequest->SetVerb(TEXT("DELETE"));
		break;

	default:
		unimplemented();
	}

	if (!AuthToken.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));
	}

	if (!Content.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		HttpRequest->SetContentAsString(Content);
	}

	return HttpRequest;
}

FString UXsollaStoreSubsystem::SerializeJson(const TSharedPtr<FJsonObject> DataJson) const
{
	FString JsonContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonContent);
	FJsonSerializer::Serialize(DataJson.ToSharedRef(), Writer);
	return JsonContent;
}

bool UXsollaStoreSubsystem::ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject) const
{
	TArray<FString> TokenParts;
	Token.ParseIntoArray(TokenParts, TEXT("."));

	FString PayloadStr;
	if (!FBase64::Decode(TokenParts[1], PayloadStr))
	{
		return false;
	}

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PayloadStr);
	if (!FJsonSerializer::Deserialize(Reader, PayloadJsonObject))
	{
		return false;
	}

	return true;
}

void UXsollaStoreSubsystem::ProcessNextCartRequest()
{
	// Cleanup finished requests firts
	int32 CartRequestsNum = CartRequestsQueue.Num();
	for (int32 i = CartRequestsNum - 1; i >= 0; --i)
	{
		if (CartRequestsQueue[i].Get().GetStatus() == EHttpRequestStatus::Succeeded ||
			CartRequestsQueue[i].Get().GetStatus() == EHttpRequestStatus::Failed ||
			CartRequestsQueue[i].Get().GetStatus() == EHttpRequestStatus::Failed_ConnectionError)
		{
			CartRequestsQueue.RemoveAt(i);
		}
	}

	// Check we have request in progress
	bool bRequestInProcess = false;
	for (int32 i = 0; i < CartRequestsQueue.Num(); ++i)
	{
		if (CartRequestsQueue[i].Get().GetStatus() == EHttpRequestStatus::Processing)
		{
			bRequestInProcess = true;
		}
	}

	// Launch next one if we have it
	if (!bRequestInProcess && CartRequestsQueue.Num() > 0)
	{
		CartRequestsQueue[0].Get().ProcessRequest();
	}
}

FString UXsollaStoreSubsystem::GetPublishingPlatformName()
{
	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();

	FString platform;

	switch (Settings->PublishingPlatform)
	{
	case EXsollaPublishingPlatform::PlaystationNetwork:
		platform = TEXT("playstation_network");
		break;

	case EXsollaPublishingPlatform::XboxLive:
		platform = TEXT("xbox_live");
		break;

	case EXsollaPublishingPlatform::Xsolla:
		platform = TEXT("xsolla");
		break;

	case EXsollaPublishingPlatform::PcStandalone:
		platform = TEXT("pc_standalone");
		break;

	case EXsollaPublishingPlatform::NintendoShop:
		platform = TEXT("nintendo_shop");
		break;

	case EXsollaPublishingPlatform::GooglePlay:
		platform = TEXT("google_play");
		break;

	case EXsollaPublishingPlatform::AppStoreIos:
		platform = TEXT("app_store_ios");
		break;

	case EXsollaPublishingPlatform::AndroidStandalone:
		platform = TEXT("android_standalone");
		break;

	case EXsollaPublishingPlatform::IosStandalone:
		platform = TEXT("ios_standalone");
		break;

	case EXsollaPublishingPlatform::AndroidOther:
		platform = TEXT("android_other");
		break;

	case EXsollaPublishingPlatform::IosOther:
		platform = TEXT("ios_other");
		break;

	case EXsollaPublishingPlatform::PcOther:
		platform = TEXT("pc_other");
		break;

	default:
		platform = TEXT("");
	}

	return platform;
}

TArray<FStoreItem> UXsollaStoreSubsystem::GetVirtualItems(const FString& GroupFilter) const
{
	if (GroupFilter.IsEmpty())
	{
		return ItemsData.Items;
	}
	else
	{
		return ItemsData.Items.FilterByPredicate([GroupFilter](const FStoreItem& InStoreItem) {
			for (auto& ItemGroup : InStoreItem.groups)
			{
				if (ItemGroup.external_id == GroupFilter)
				{
					return true;
				}
			}
			return false;
		});
	}
}

TArray<FStoreItem> UXsollaStoreSubsystem::GetVirtualItemsWithoutGroup() const
{
	return ItemsData.Items.FilterByPredicate([](const FStoreItem& InStoreItem) {
		return InStoreItem.groups.Num() == 0;
	});
}

FStoreItemsData UXsollaStoreSubsystem::GetItemsData() const
{
	return ItemsData;
}

TArray<FVirtualCurrency> UXsollaStoreSubsystem::GetVirtualCurrencyData() const
{
	return VirtualCurrencyData.Items;
}

TArray<FVirtualCurrencyPackage> UXsollaStoreSubsystem::GetVirtualCurrencyPackages() const
{
	return VirtualCurrencyPackages.Items;
}

TArray<FVirtualCurrencyBalance> UXsollaStoreSubsystem::GetVirtualCurrencyBalance() const
{
	return VirtualCurrencyBalance.Items;
}

FStoreCart UXsollaStoreSubsystem::GetCart() const
{
	return Cart;
}

FStoreInventory UXsollaStoreSubsystem::GetInventory() const
{
	return Inventory;
}

FString UXsollaStoreSubsystem::GetPendingPaystationUrl() const
{
	return PengindPaystationUrl;
}

UDataTable* UXsollaStoreSubsystem::GetCurrencyLibrary() const
{
	return CurrencyLibrary;
}

UXsollaStoreImageLoader* UXsollaStoreSubsystem::GetImageLoader() const
{
	return ImageLoader;
}

void UXsollaStoreSubsystem::LoadImageFromWeb(const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	GetImageLoader()->LoadImage(URL, SuccessCallback, ErrorCallback);
}

FString UXsollaStoreSubsystem::FormatPrice(float Amount, const FString& Currency) const
{
	auto Row = GetCurrencyLibrary()->FindRow<FXsollaStoreCurrency>(FName(*Currency), FString());
	if (Row)
	{
		FString SanitizedAmount = UKismetTextLibrary::Conv_FloatToText(Amount, ERoundingMode::HalfToEven, false, true, 1, 324, Row->fractionSize, Row->fractionSize).ToString();
		return Row->symbol.format.Replace(TEXT("$"), *Row->symbol.grapheme).Replace(TEXT("1"), *SanitizedAmount);
	}

	UE_LOG(LogXsollaStore, Error, TEXT("%s: Failed to format price (%d %s)"), *VA_FUNC_LINE, Amount, *Currency);
	return FString();
}

#undef LOCTEXT_NAMESPACE
