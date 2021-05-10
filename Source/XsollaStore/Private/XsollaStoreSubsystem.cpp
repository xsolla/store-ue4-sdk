// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaStoreSubsystem.h"

#include "XsollaStore.h"
#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreSave.h"
#include "XsollaStoreSettings.h"
#include "XsollaUtilsLibrary.h"

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

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

UXsollaStoreSubsystem::UXsollaStoreSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(
		TEXT("/Xsolla/Store/Components/W_StoreBrowser.W_StoreBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
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

void UXsollaStoreSubsystem::UpdateVirtualItems(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_items?locale=%s&country=%s%s&limit=%d&offset=%d"),
		*ProjectID,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString,
		Limit,
		Offset);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateVirtualItems_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateItemGroups(const FString& Locale,
	const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/groups?locale=%s&limit=%d&offset=%d"),
		*ProjectID,
		*UsedLocale,
		Limit,
		Offset);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateItemGroups_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencies(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency?locale=%s&country=%s%s&limit=%d&offset=%d"),
		*ProjectID,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString,
		Limit,
		Offset);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateVirtualCurrencies_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/package?locale=%s&country=%s%s&limit=%d&offset=%d"),
		*ProjectID,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString,
		Limit,
		Offset);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetItemsListBySpecifiedGroup(const FString& ExternalId,
	const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnGetItemsListBySpecifiedGroup& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_items/group/%s?locale=%s&country=%s%s&limit=%d&offset=%d"),
		*ProjectID,
		ExternalId.IsEmpty() ? *FString(TEXT("all")) : *ExternalId,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString,
		Limit,
		Offset);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetItemsListBySpecifiedGroup_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU,
	const FString& Currency, const FString& Country, const FString& Locale,
	const FXsollaParameters CustomParameters,
	const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	TSharedPtr<FJsonObject> RequestDataJson = PreparePaymentTokenRequestPayload(Currency, Country, Locale, CustomParameters);

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/payment/item/%s"),
		*ProjectID,
		*ItemSKU);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken, SerializeJson(RequestDataJson));

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();

	if (Settings->BuildForSteam)
	{
		FString SteamId;
		FString OutError;

		if (!GetSteamUserId(AuthToken, SteamId, OutError))
		{
			ErrorCallback.ExecuteIfBound(0, 0, OutError);
			return;
		}

		HttpRequest->SetHeader(TEXT("x-steam-userid"), SteamId);
	}

	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::FetchCartPaymentToken(const FString& AuthToken, const FString& CartId,
	const FString& Currency, const FString& Country, const FString& Locale,
	const FXsollaParameters CustomParameters,
	const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;

	TSharedPtr<FJsonObject> RequestDataJson = PreparePaymentTokenRequestPayload(Currency, Country, Locale, CustomParameters);

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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken, SerializeJson(RequestDataJson));

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();

	if (Settings->BuildForSteam)
	{
		FString SteamId;
		FString OutError;

		if (!GetSteamUserId(AuthToken, SteamId, OutError))
		{
			ErrorCallback.ExecuteIfBound(0, 0, OutError);
			return;
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

		if (MyBrowser == nullptr || !MyBrowser->IsValidLowLevel() || !MyBrowser->GetIsEnabled())
		{
			MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
			MyBrowser->AddToViewport(MAX_int32);
		}
		else
		{
			MyBrowser->SetVisibility(ESlateVisibility::Visible);
		}

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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_PUT, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::ClearCart_HttpRequestComplete, SuccessCallback, ErrorCallback);

	CartRequestsQueue.Add(HttpRequest);
	ProcessNextCartRequest();

	// Just cleanup local cart
	Cart.Items.Empty();
	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreSubsystem::UpdateCart(const FString& AuthToken, const FString& CartId,
	const FString& Currency, const FString& Locale,
	const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	CachedAuthToken = AuthToken;
	CachedCartId = CartId;
	CachedCartCurrency = Currency;
	CachedCartLocale = Locale;

	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	FString Url;
	if (CartId.IsEmpty())
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart?currency=%s&locale=%s"),
			*ProjectID,
			*Currency,
			*UsedLocale);
	}
	else
	{
		Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/cart/%s?currency=%s&locale=%s"),
			*ProjectID,
			*Cart.cart_id,
			*Currency,
			*UsedLocale);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_PUT, AuthToken, SerializeJson(RequestDataJson));
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_DELETE, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_PUT, AuthToken, SerializeJson(JsonObject));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::FillCartById_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetSpecifiedBundle(const FString& Sku, const FOnGetSpecifiedBundleUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/bundle/sku/%s"), *ProjectID, *Sku);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetSpecifiedBundle_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::UpdateBundles(const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnGetListOfBundlesUpdate& SuccessCallback, const FOnStoreError& ErrorCallback, const int Limit, const int Offset)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/bundle?locale=%s&country=%s%s&limit=%d&offset=%d"),
		*ProjectID,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString,
		Limit,
		Offset);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetListOfBundles_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetVirtualCurrency(const FString& CurrencySKU,
	const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnCurrencyUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/sku/%s?locale=%s&country=%s%s"),
		*ProjectID,
		*CurrencySKU,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetVirtualCurrency_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetVirtualCurrencyPackage(const FString& PackageSKU,
	const FString& Locale, const FString& Country, const TArray<FString>& AdditionalFields,
	const FOnCurrencyPackageUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString UsedLocale = Locale.IsEmpty() ? TEXT("en") : Locale;
	const FString AdditionalFieldsString = ConvertAdditionalFieldsToString(AdditionalFields);
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/items/virtual_currency/package/sku/%s?locale=%s&country=%s%s"),
		*ProjectID,
		*PackageSKU,
		*UsedLocale,
		*Country,
		AdditionalFieldsString.IsEmpty() ? TEXT("") : *AdditionalFieldsString);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::BuyItemWithVirtualCurrency_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::GetPromocodeRewards(const FString& AuthToken, const FString& PromocodeCode,
	const FOnGetPromocodeRewardsUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/promocode/code/%s/rewards"),
		*ProjectID,
		*PromocodeCode);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::GetPromocodeRewards_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreSubsystem::RedeemPromocode(const FString& AuthToken, const FString& PromocodeCode,
	const FOnRedeemPromocodeUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/promocode/redeem"), *ProjectID);

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("coupon_code"), PromocodeCode);

	TSharedPtr<FJsonObject> JsonCart = MakeShareable(new FJsonObject);
	JsonCart->SetStringField(TEXT("id"), Cart.cart_id);
	RequestDataJson->SetObjectField(TEXT("cart"), JsonCart);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaStoreSubsystem::RedeemPromocode_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

FStoreBattlepassData UXsollaStoreSubsystem::ParseBattlepass(const FString& BattlepassInfo)
{
	FStoreBattlepassData BattlepassData;

	TSharedPtr<FJsonObject> JsonObject;

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(BattlepassInfo, &BattlepassData, 0, 0))
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't convert BattlepassInfo to struct"), *VA_FUNC_LINE);
	}

	return BattlepassData;
}

void UXsollaStoreSubsystem::UpdateVirtualItems_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreItemsData::StaticStruct(), &ItemsData, OutError))
	{
		// Update categories
		for (auto& Item : ItemsData.Items)
		{
			for (auto& ItemGroup : Item.groups)
			{
				ItemsData.GroupIds.Add(ItemGroup.external_id);
			}
		}

		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::UpdateItemGroups_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FStoreItemsData GroupsData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreItemsData::StaticStruct(), &GroupsData, OutError))
	{
		ItemsData.Groups = GroupsData.Groups;
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencies_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrencyData::StaticStruct(), &VirtualCurrencyData, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::UpdateVirtualCurrencyPackages_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrencyPackagesData::StaticStruct(), &VirtualCurrencyPackages, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::GetItemsListBySpecifiedGroup_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnGetItemsListBySpecifiedGroup SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FStoreItemsList Items;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreItemsList::StaticStruct(), &Items, OutError))
	{
		SuccessCallback.ExecuteIfBound(Items);
	}
	else
	{
		ProcessNextCartRequest();
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::FetchPaymentToken_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		FString AccessToken = JsonObject->GetStringField(TEXT("token"));
		int32 OrderId = JsonObject->GetNumberField(TEXT("order_id"));

		SuccessCallback.ExecuteIfBound(AccessToken, OrderId);
		return;
	}

	HandleRequestError(OutError, ErrorCallback);
}

void UXsollaStoreSubsystem::CheckOrder_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnCheckOrder SuccessCallback, FOnStoreError ErrorCallback)
{
	FXsollaOrder Order;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaOrder::StaticStruct(), &Order, OutError))
	{
		FString OrderStatusStr = Order.status;

		EXsollaOrderStatus OrderStatus = EXsollaOrderStatus::Unknown;

		if (OrderStatusStr == TEXT("new"))
		{
			OrderStatus = EXsollaOrderStatus::New;
		}
		else if (OrderStatusStr == TEXT("paid"))
		{
			OrderStatus = EXsollaOrderStatus::Paid;
		}
		else if (OrderStatusStr == TEXT("done"))
		{
			OrderStatus = EXsollaOrderStatus::Done;
		}
		else if (OrderStatusStr == TEXT("canceled"))
		{
			OrderStatus = EXsollaOrderStatus::Canceled;
		}
		else
		{
			UE_LOG(LogXsollaStore, Warning, TEXT("%s: Unknown order status: %s [%d]"), *VA_FUNC_LINE, *OrderStatusStr, Order.order_id);
		}

		SuccessCallback.ExecuteIfBound(Order.order_id, OrderStatus, Order.content);
		return;
	}

	HandleRequestError(OutError, ErrorCallback);
}

void UXsollaStoreSubsystem::CreateCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		Cart = FStoreCart(JsonObject->GetStringField(TEXT("cart_id")));
		OnCartUpdate.Broadcast(Cart);

		SaveData();

		SuccessCallback.ExecuteIfBound();
		return;
	}

	ProcessNextCartRequest();
	HandleRequestError(OutError, ErrorCallback);
}

void UXsollaStoreSubsystem::ClearCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}

	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::UpdateCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreCart::StaticStruct(), &Cart, OutError))
	{
		OnCartUpdate.Broadcast(Cart);
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}

	ProcessNextCartRequest();
}

void UXsollaStoreSubsystem::AddToCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		ProcessNextCartRequest();
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		UpdateCart(CachedAuthToken, CachedCartId, CachedCartCurrency, CachedCartLocale, SuccessCallback, ErrorCallback);
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::RemoveFromCart_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		ProcessNextCartRequest();
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		UpdateCart(CachedAuthToken, CachedCartId, CachedCartCurrency, CachedCartLocale, SuccessCallback, ErrorCallback);
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::FillCartById_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreCart::StaticStruct(), &Cart, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::GetListOfBundles_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnGetListOfBundlesUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FStoreListOfBundles ListOfBundles;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreListOfBundles::StaticStruct(), &ListOfBundles, OutError))
	{
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
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::GetSpecifiedBundle_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnGetSpecifiedBundleUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FStoreBundle Bundle;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreBundle::StaticStruct(), &Bundle, OutError))
	{
		SuccessCallback.ExecuteIfBound(Bundle);
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::GetVirtualCurrency_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnCurrencyUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FVirtualCurrency currency;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrency::StaticStruct(), &currency, OutError))
	{
		SuccessCallback.ExecuteIfBound(currency);
	}
	else
	{
		ProcessNextCartRequest();
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::GetVirtualCurrencyPackage_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnCurrencyPackageUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FVirtualCurrencyPackage currencyPackage;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrencyPackage::StaticStruct(), &currencyPackage, OutError))
	{
		SuccessCallback.ExecuteIfBound(currencyPackage);
	}
	else
	{
		ProcessNextCartRequest();
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::BuyItemWithVirtualCurrency_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnPurchaseUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		int32 OrderId = JsonObject->GetNumberField(TEXT("order_id"));
		SuccessCallback.ExecuteIfBound(OrderId);
		return;
	}

	HandleRequestError(OutError, ErrorCallback);
}

void UXsollaStoreSubsystem::GetPromocodeRewards_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnGetPromocodeRewardsUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FStorePromocodeRewardData PromocodeRewardData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStorePromocodeRewardData::StaticStruct(), &PromocodeRewardData, OutError))
	{
		SuccessCallback.ExecuteIfBound(PromocodeRewardData);
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::RedeemPromocode_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnRedeemPromocodeUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FStorePromocodeRewardData PromocodeRewardData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FStoreCart::StaticStruct(), &Cart, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaStoreSubsystem::HandleRequestError(XsollaHttpRequestError ErrorData, FOnStoreError ErrorCallback)
{
	UE_LOG(LogXsollaStore, Error, TEXT("%s: request failed - Status code: %d, Error code: %d, Error message: %s"),
		*VA_FUNC_LINE, ErrorData.statusCode, ErrorData.errorCode, *ErrorData.errorMessage);
	ErrorCallback.ExecuteIfBound(ErrorData.statusCode, ErrorData.errorCode, ErrorData.errorMessage);
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

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaStoreSubsystem::CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb,
	const FString& AuthToken, const FString& Content)
{
	return XsollaUtilsHttpRequestHelper::CreateHttpRequest(Url, Verb, AuthToken, Content, TEXT("STORE"), XSOLLA_STORE_VERSION);
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

TSharedPtr<FJsonObject> UXsollaStoreSubsystem::PreparePaymentTokenRequestPayload(
	const FString& Currency, const FString& Country, const FString& Locale, const FXsollaParameters CustomParameters)
{
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();

	// General
	if (!Currency.IsEmpty())
		RequestDataJson->SetStringField(TEXT("currency"), Currency);
	if (!Country.IsEmpty())
		RequestDataJson->SetStringField(TEXT("country"), Country);
	if (!Locale.IsEmpty())
		RequestDataJson->SetStringField(TEXT("locale"), Locale);

	// Sandbox
	RequestDataJson->SetBoolField(TEXT("sandbox"), IsSandboxEnabled());

	// Custom parameters
	UXsollaUtilsLibrary::AddParametersToJsonObjectByFieldName(RequestDataJson, "custom_parameters", CustomParameters);

	// PayStation settings
	TSharedPtr<FJsonObject> PaymentSettingsJson = MakeShareable(new FJsonObject);

	TSharedPtr<FJsonObject> PaymentUiSettingsJson = MakeShareable(new FJsonObject);

	PaymentUiSettingsJson->SetStringField(TEXT("theme"),
		Settings->PaymentInterfaceTheme != EXsollaPaymentUiTheme::default_light
			? UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPaymentUiTheme", Settings->PaymentInterfaceTheme)
			: TEXT("default"));
	PaymentUiSettingsJson->SetStringField(TEXT("size"),
		UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPaymentUiSize", Settings->PaymentInterfaceSize));

	if (Settings->PaymentInterfaceVersion != EXsollaPaymentUiVersion::not_specified)
		PaymentUiSettingsJson->SetStringField(TEXT("version"),
			UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPaymentUiVersion", Settings->PaymentInterfaceVersion));

	PaymentSettingsJson->SetObjectField(TEXT("ui"), PaymentUiSettingsJson);

	if (Settings->OverrideRedirectPolicy)
	{
		if (!Settings->ReturnUrl.IsEmpty())
			PaymentSettingsJson->SetStringField(TEXT("return_url"), Settings->ReturnUrl);

		TSharedPtr<FJsonObject> RedirectSettingsJson = MakeShareable(new FJsonObject);

		RedirectSettingsJson->SetStringField(TEXT("redirect_conditions"),
			UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPaymentRedirectCondition", Settings->RedirectCondition));
		RedirectSettingsJson->SetStringField(TEXT("status_for_manual_redirection"),
			UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPaymentRedirectStatusManual", Settings->RedirectStatusManual));

		RedirectSettingsJson->SetNumberField(TEXT("delay"), Settings->RedirectDelay);
		RedirectSettingsJson->SetStringField(TEXT("redirect_button_caption"), Settings->RedirectButtonCaption);

		PaymentSettingsJson->SetObjectField(TEXT("redirect_policy"), RedirectSettingsJson);
	}

	RequestDataJson->SetObjectField(TEXT("settings"), PaymentSettingsJson);

	return RequestDataJson;
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
		UE_LOG(LogXsollaStore, Warning, TEXT("%s: There is no default publishing platform value"), *VA_FUNC_LINE);
	}

	return platform;
}

FString UXsollaStoreSubsystem::ConvertAdditionalFieldsToString(const TArray<FString>& AdditionalFields)
{
	if (AdditionalFields.Num() == 0)
	{
		return TEXT("");
	}

	FString AdditionalFieldsString = FString::JoinBy(AdditionalFields, TEXT("&"), [](const FString& AdditionalField) {
		return FString::Printf(TEXT("additional_fields[]=%s"), *AdditionalField);
	});
	AdditionalFieldsString.InsertAt(0, TEXT("&"));
	AdditionalFieldsString.RemoveFromEnd(TEXT("&"));

	return AdditionalFieldsString;
}

bool UXsollaStoreSubsystem::GetSteamUserId(const FString& AuthToken, FString& SteamId, FString& OutError)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(AuthToken, PayloadJsonObject))
	{
		OutError = TEXT("Can't parse token payload");
		UE_LOG(LogXsollaStore, Error, TEXT("%s: %s"), *VA_FUNC_LINE, *OutError);
		return false;
	}

	FString SteamIdUrl;
	if (!PayloadJsonObject->TryGetStringField(TEXT("id"), SteamIdUrl))
	{
		OutError = TEXT("Can't find Steam profile ID in token payload");
		UE_LOG(LogXsollaStore, Error, TEXT("%s: %s"), *VA_FUNC_LINE, *OutError);
		return false;
	}

	// Extract ID value from user's Steam profile URL
	int SteamIdIndex;
	if (SteamIdUrl.FindLastChar('/', SteamIdIndex))
	{
		SteamId = SteamIdUrl.RightChop(SteamIdIndex + 1);
	}

	return true;
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

FVirtualCurrency UXsollaStoreSubsystem::FindVirtualCurrencyBySku(const FString& CurrencySku, bool& bHasFound) const
{
	FVirtualCurrency VirtualCurrency;
	bHasFound = false;

	const auto Currency = VirtualCurrencyData.Items.FindByPredicate([CurrencySku](const FVirtualCurrency& InCurrency) {
		return InCurrency.sku == CurrencySku;
	});

	if (Currency != nullptr)
	{
		VirtualCurrency = Currency[0];
		bHasFound = true;
	}

	return VirtualCurrency;
}

FStoreItem UXsollaStoreSubsystem::FindItemBySku(const FString& ItemSku, bool& bHasFound) const
{
	FStoreItem Item;
	bHasFound = false;

	const auto StoreItem = ItemsData.Items.FindByPredicate([ItemSku](const FStoreItem& InItem) {
		return InItem.sku == ItemSku;
	});

	if (StoreItem != nullptr)
	{
		Item = StoreItem[0];
		bHasFound = true;
	}

	return Item;
}

FVirtualCurrencyPackage UXsollaStoreSubsystem::FindVirtualCurrencyPackageBySku(const FString& ItemSku, bool& bHasFound) const
{
	FVirtualCurrencyPackage Package;
	bHasFound = false;

	const auto PackageItem = VirtualCurrencyPackages.Items.FindByPredicate([ItemSku](const FVirtualCurrencyPackage& InItem) {
		return InItem.sku == ItemSku;
	});

	if (PackageItem != nullptr)
	{
		Package = PackageItem[0];
		bHasFound = true;
	}

	return Package;
}

bool UXsollaStoreSubsystem::IsItemInCart(const FString& ItemSKU) const
{
	auto CartItem = Cart.Items.FindByPredicate([ItemSKU](const FStoreCartItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	return CartItem != nullptr;
}

#undef LOCTEXT_NAMESPACE
