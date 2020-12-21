// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreSubsystem.h"

#include "XsollaStore.h"
#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreSave.h"
#include "XsollaStoreSettings.h"

#include "Dom/JsonObject.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "JsonObjectConverter.h"
#include "Misc/Base64.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"
#include "XsollaUtilsLibrary.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

UXsollaStoreSubsystem::UXsollaStoreSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(
		TEXT("/Xsolla/Store/Components/W_StoreBrowser.W_StoreBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;

	// @TODO https://github.com/xsolla/store-ue4-sdk/issues/68
	CachedCartCurrency = TEXT("USD");
}

void UXsollaStoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize subsystem with project identifier provided by user
	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
	Initialize(Settings->ProjectID);

	UE_LOG(LogXsollaStore, Log, TEXT("%s: XsollaStore subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaStoreSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaStoreSubsystem::Initialize(const FString& InProjectId)
{
	ProjectID = InProjectId;

	LoadData();
}

void UXsollaStoreSubsystem::UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_items?offset=%d&limit=%d"),
		*ProjectID, Offset, Limit);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateVirtualItems_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateItemGroups(const FString& Locale,
	const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/groups?locale=%s&offset=%d&limit=%d"),
		*ProjectID, *UsedLocale, Offset, Limit);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateItemGroups_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencies(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency?offset=%d&limit=%d"),
		*ProjectID, Offset, Limit);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateVirtualCurrencies_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/package?offset=%d&limit=%d"),
		*ProjectID, Offset, Limit);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetItemsListBySpecifiedGroup(
	const FString& ExternalId, const int Limit, const int Offset,
	const FString& Locale, const TArray<FString>& AdditionalFields,
	const FOnGetItemsListBySpecifiedGroup& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	FString StringAdditionalFields = FString::Join(AdditionalFields, TEXT(","));
	StringAdditionalFields.RemoveFromEnd(",");

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_items/group/%s?locale=%s&limit=%d&offset=%d%s"),
		*ProjectID,
		ExternalId.IsEmpty() ? *FString(TEXT("all")) : *ExternalId,
		Locale.IsEmpty() ? *FString(TEXT("en")) : *Locale,
		Limit,
		Offset,
		StringAdditionalFields.IsEmpty() ? *FString(TEXT("")) : *FString(TEXT("&additional_fields[]=") + StringAdditionalFields));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetItemsListBySpecifiedGroup_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU,
	const FString& Currency, const FString& Country, const FString& Locale,
	const FXsollaPaymentCustomParameters CustomParameters,
	const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
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

	AddCustomParameters(RequestDataJson, CustomParameters);

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

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/item/%s"),
		*ProjectID,
		*ItemSKU);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));

	if (Settings->BuildForSteam)
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

	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::FetchCartPaymentToken(const FString& AuthToken, const FString& CartId,
	const FString& Currency, const FString& Country, const FString& Locale,
	const FXsollaPaymentCustomParameters CustomParameters,
	const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
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

	AddCustomParameters(RequestDataJson, CustomParameters);

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
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/cart"),
			*ProjectID);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/cart/%s"),
			*ProjectID,
			*Cart.cart_id);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));

	if (Settings->BuildForSteam)
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

	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
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
	if (Settings->UsePlatformBrowser)
	{
		UE_LOG(LogXsollaStore, Log, TEXT("%s: Launching Paystation: %s"), *VA_FUNC_LINE, *PaystationUrl);

		BrowserWidget = nullptr;

		FPlatformProcess::LaunchURL(*PaystationUrl, nullptr, nullptr);
	}
	else
	{
		UE_LOG(LogXsollaStore, Log, TEXT("%s: Loading Paystation: %s"), *VA_FUNC_LINE, *PaystationUrl);

		// Check for user browser widget override
		auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass)
									  ? Settings->OverrideBrowserWidgetClass
									  : DefaultBrowserWidgetClass;

		PengindPaystationUrl = PaystationUrl;
		auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
		MyBrowser->AddToViewport(MAX_int32);

		BrowserWidget = MyBrowser;
	}
}

void UXsollaStoreSubsystem::CheckOrder(const FString& AuthToken, int32 OrderId,
	const FOnCheckOrder& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/order/%d"),
		*ProjectID,
		OrderId);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::CheckOrder_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::ClearCart(const FString& AuthToken, const FString& CartId,
	const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/clear"),
			*ProjectID);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/clear"),
			*ProjectID,
			*Cart.cart_id);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::PUT, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::ClearCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();

	// Just cleanup local cart
	Cart.Items.Empty();
	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreSubsystem::UpdateCart(const FString& AuthToken, const FString& CartId,
	const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart"),
			*ProjectID);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s"),
			*ProjectID,
			*Cart.cart_id);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::AddToCart(const FString& AuthToken, const FString& CartId,
	const FString& ItemSKU, int32 Quantity,
	const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetNumberField(TEXT("quantity"), Quantity);

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/item/%s"),
			*ProjectID,
			*ItemSKU);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/item/%s"),
			*ProjectID,
			*Cart.cart_id,
			*ItemSKU);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::PUT, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::AddToCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

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

void UXsollaStoreSubsystem::RemoveFromCart(const FString& AuthToken, const FString& CartId, const FString& ItemSKU,
	const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/item/%s"),
			*ProjectID,
			*ItemSKU);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/item/%s"),
			*ProjectID,
			*Cart.cart_id,
			*ItemSKU);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::DELETE, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::RemoveFromCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

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

void UXsollaStoreSubsystem::FillCartById(const FString& AuthToken, const FString& CartId, const TArray<FStoreCartItem>& Items,
	const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/fill"),
			*ProjectID);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s/fill"),
			*ProjectID, *CartId);
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> JsonItems;

	for (auto& Item : Items)
	{
		TSharedPtr<FJsonObject> JsonItem = MakeShareable(new FJsonObject());
		JsonItem->SetStringField(TEXT("sku"), Item.sku);
		JsonItem->SetNumberField(TEXT("quantity"), Item.quantity);
		JsonItems.Push(MakeShareable(new FJsonValueObject(JsonItem)));
	}

	JsonObject->SetArrayField(TEXT("items"), JsonItems);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::PUT, AuthToken, SerializeJson(JsonObject));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::FillCartById_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetSpecifiedBundle(const FString& Sku, const FOnGetSpecifiedBundleUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/bundle/sku/%s"), *ProjectID, *Sku);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetSpecifiedBundle_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateBundles(const FString& Locale, const FOnGetListOfBundlesUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/bundle?locale=%s"), *ProjectID, *UsedLocale);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetListOfBundles_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetVirtualCurrency(const FString& CurrencySKU,
	const FOnCurrencyUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/sku/%s"),
		*ProjectID,
		*CurrencySKU);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetVirtualCurrency_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetVirtualCurrencyPackage(const FString& PackageSKU,
	const FOnCurrencyPackageUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/package/sku/%s"),
		*ProjectID,
		*PackageSKU);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetVirtualCurrencyPackage_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::BuyItemWithVirtualCurrency(const FString& AuthToken,
	const FString& ItemSKU, const FString& CurrencySKU,
	const FOnPurchaseUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;

	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/item/%s/virtual/%s"),
		*ProjectID,
		*ItemSKU,
		*CurrencySKU);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::BuyItemWithVirtualCurrency_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetPromocodeRewards(const FString& AuthToken, const FString& PromocodeCode, const FOnGetPromocodeRewardsUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/promocode/code/%s/rewards"),
		*ProjectID,
		*PromocodeCode);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetPromocodeRewards_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::RedeemPromocode(const FString& AuthToken, const FString& PromocodeCode, const FOnRedeemPromocodeUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/promocode/redeem"), *ProjectID);

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("coupon_code"), PromocodeCode);

	TSharedPtr<FJsonObject> JsonCart = MakeShareable(new FJsonObject);
	JsonCart->SetStringField(TEXT("id"), Cart.cart_id);
	RequestDataJson->SetObjectField(TEXT("cart"), JsonCart);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::RedeemPromocode_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualItems_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::UpdateItemGroups_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::UpdateVirtualCurrencies_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::GetItemsListBySpecifiedGroup_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnGetItemsListBySpecifiedGroup SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		ProcessNextCartRequest();
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FStoreItemsList Items;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreItemsList::StaticStruct(), &Items))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound(Items);
}

void UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::CheckOrder_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnCheckOrder SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::CreateCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::ClearCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::UpdateCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::AddToCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::RemoveFromCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::FillCartById_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
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

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreSubsystem::GetListOfBundles_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnGetListOfBundlesUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FStoreListOfBundles ListOfBundles;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreListOfBundles::StaticStruct(), &ListOfBundles))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	for (auto Bundle : ListOfBundles.items)
	{
		ItemsData.Items.Add(Bundle);
	}

	for (auto Bundle : ListOfBundles.items)
	{
		for (auto BundleGroup : Bundle.groups)
		{
			ItemsData.GroupIds.Add(BundleGroup.external_id);
		}
	}

	SuccessCallback.ExecuteIfBound(ListOfBundles);
}

void UXsollaStoreSubsystem::GetSpecifiedBundle_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnGetSpecifiedBundleUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FStoreBundle Bundle;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStoreBundle::StaticStruct(), &Bundle))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound(Bundle);
}

void UXsollaStoreSubsystem::GetVirtualCurrency_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnCurrencyUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::GetVirtualCurrencyPackage_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnCurrencyPackageUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::BuyItemWithVirtualCurrency_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

void UXsollaStoreSubsystem::GetPromocodeRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnGetPromocodeRewardsUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FStorePromocodeRewardData PromocodeRewardData;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FStorePromocodeRewardData::StaticStruct(), &PromocodeRewardData))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound(PromocodeRewardData);
}

void UXsollaStoreSubsystem::RedeemPromocode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRedeemPromocodeUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
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

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

bool UXsollaStoreSubsystem::HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreError ErrorCallback)
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
	bool bIsSandboxEnabled = Settings->EnableSandbox;

#if UE_BUILD_SHIPPING
	bIsSandboxEnabled = Settings->EnableSandbox && Settings->EnableSandboxInShippingBuild;
	if (bIsSandboxEnabled)
	{
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: Sandbox should be disabled in Shipping build"), *VA_FUNC_LINE);
	}
#endif // UE_BUILD_SHIPPING

	return bIsSandboxEnabled;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaStoreSubsystem::CreateHttpRequest(const FString& Url, const EXsollaRequestVerb Verb,
	const FString& AuthToken, const FString& Content)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

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

	if (!Settings->UseCrossPlatformAccountLinking)
	{
		return platform;
	}

	switch (Settings->Platform)
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

FStoreCart UXsollaStoreSubsystem::GetCart() const
{
	return Cart;
}

FString UXsollaStoreSubsystem::GetPendingPaystationUrl() const
{
	return PengindPaystationUrl;
}

FString UXsollaStoreSubsystem::GetItemName(const FString& ItemSKU) const
{
	auto StoreItem = ItemsData.Items.FindByPredicate([ItemSKU](const FStoreItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	if (StoreItem != nullptr)
	{
		return StoreItem->name;
	}

	return TEXT("");
}

FString UXsollaStoreSubsystem::GetVirtualCurrencyName(const FString& CurrencySKU) const
{
	auto Currency = VirtualCurrencyData.Items.FindByPredicate([CurrencySKU](const FVirtualCurrency& InCurrency) {
		return InCurrency.sku == CurrencySKU;
	});

	if (Currency != nullptr)
	{
		return Currency->name;
	}

	return TEXT("");
}

bool UXsollaStoreSubsystem::IsItemInCart(const FString& ItemSKU) const
{
	auto CartItem = Cart.Items.FindByPredicate([ItemSKU](const FStoreCartItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	return CartItem != nullptr;
}

void UXsollaStoreSubsystem::AddCustomParameters(TSharedPtr<FJsonObject> JsonObject, FXsollaPaymentCustomParameters CustomParameters)
{
	if (CustomParameters.Parameters.Num() == 0)
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObjectCustomParameters = MakeShareable(new FJsonObject());

	for (auto Parameter : CustomParameters.Parameters)
	{
		const FVariant Variant = Parameter.Value.Variant;

		switch (Variant.GetType())
		{
		case EVariantTypes::Bool:
			JsonObjectCustomParameters->SetBoolField(Parameter.Key, Variant.GetValue<bool>());
			break;
		case EVariantTypes::String:
			JsonObjectCustomParameters->SetStringField(Parameter.Key, Variant.GetValue<FString>());
			break;
		case EVariantTypes::Int32:
			JsonObjectCustomParameters->SetNumberField(Parameter.Key, Variant.GetValue<int>());
			break;
		case EVariantTypes::Float:
			JsonObjectCustomParameters->SetNumberField(Parameter.Key, Variant.GetValue<float>());
			break;
		default:
			UE_LOG(LogXsollaStore, Log, TEXT("%s: parameter with type of %s was not added"),
				*VA_FUNC_LINE, *UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaVariantTypes", static_cast<EXsollaVariantTypes>(Variant.GetType())));
			break;
		}
	}

	JsonObject->SetObjectField("custom_parameters", JsonObjectCustomParameters);
}

#undef LOCTEXT_NAMESPACE
