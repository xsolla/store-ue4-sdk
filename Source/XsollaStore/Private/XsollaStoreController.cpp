// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreController.h"

#include "XsollaStore.h"
#include "XsollaStoreBrowser.h"
#include "XsollaStoreDataModel.h"
#include "XsollaStoreDefines.h"
#include "XsollaStoreImageLoader.h"
#include "XsollaStoreSave.h"
#include "XsollaStoreSettings.h"

#include "Engine.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "Runtime/Launch/Resources/Version.h"

#define LOCTEXT_NAMESPACE "FXsollaStoreModule"

UXsollaStoreController::UXsollaStoreController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXsollaStoreController::Initialize(const FString& InProjectId)
{
	ProjectId = InProjectId;

	LoadData();

	// Check image loader is exsits, because initialization can be called multiple times
	if (!ImageLoader)
	{
		ImageLoader = NewObject<UXsollaStoreImageLoader>();
	}
}

void UXsollaStoreController::UpdateVirtualItems(const FOnStoreUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/project/%s/items/virtual_items"), *ProjectId);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::UpdateVirtualItems_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreController::FetchPaymentToken(const FString& AuthToken, const FString& ItemSKU, const FString& Currency, const FString& Country, const FString& Locale, const FOnFetchTokenSuccess& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	if (!Currency.IsEmpty())
		RequestDataJson->SetStringField(TEXT("currency"), Currency);
	if (!Country.IsEmpty())
		RequestDataJson->SetStringField(TEXT("country"), Country);
	if (!Locale.IsEmpty())
		RequestDataJson->SetStringField(TEXT("locale"), Locale);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/payment/item/%s"), *ItemSKU);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("POST"));

	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetContentAsString(PostContent);

	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreController::LaunchPaymentConsole(const FString& AccessToken)
{
	FString PaystationUrl = FString::Printf(TEXT("https://secure.xsolla.com/paystation3?access_token=%s"), *AccessToken);

	const UXsollaStoreSettings* Settings = FXsollaStoreModule::Get().GetSettings();
	if (Settings->bUsePlatformBrowser)
	{
		UE_LOG(LogXsollaStore, Log, TEXT("%s: Launching Paystation: %s"), *VA_FUNC_LINE, *PaystationUrl);

		FPlatformProcess::LaunchURL(*PaystationUrl, nullptr, nullptr);
	}
	else
	{
		UE_LOG(LogXsollaStore, Log, TEXT("%s: Loading Paystation: %s"), *VA_FUNC_LINE, *PaystationUrl);

		auto MyBrowser = CreateWidget<UXsollaStoreBrowser>(GEngine->GameViewport->GetWorld(), UXsollaStoreBrowser::StaticClass());
		MyBrowser->LoadWidget();
		MyBrowser->GetBrowser()->LoadURL(PaystationUrl);
	}
}

void UXsollaStoreController::CreateCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = TEXT("https://store.xsolla.com/api/v1/cart");

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::CreateCart_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreController::ClearCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/cart/%d/clear"), Cart.cart_id);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("PUT"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::ClearCart_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();

	// Just cleanup local cart
	Cart.Items.Empty();
	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreController::UpdateCart(const FString& AuthToken, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/cart/%d"), Cart.cart_id);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::UpdateCart_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaStoreController::AddToCart(const FString& AuthToken, const FString& ItemSKU, int32 Quantity, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetNumberField(TEXT("quantity"), Quantity);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/cart/%d/item/%s"), Cart.cart_id, *ItemSKU);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("PUT"));

	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetContentAsString(PostContent);

	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::AddToCart_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();

	// Try to update item quantity
	auto CartItem = Cart.Items.FindByPredicate([ItemSKU](const FStoreItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	if (CartItem)
	{
		CartItem->quantity = Quantity;
	}
	else
	{
		auto StoreItem = ItemsData.Items.FindByPredicate([ItemSKU](const FStoreItem& InItem) {
			return InItem.sku == ItemSKU;
		});

		if (StoreItem)
		{
			FStoreItem Item(*StoreItem);
			Item.quantity = Quantity;
			Cart.Items.Add(Item);
		}
		else
		{
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't find provided SKU in local cache: %s"), *VA_FUNC_LINE, *ItemSKU);
		}
	}

	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreController::RemoveFromCart(const FString& AuthToken, const FString& ItemSKU, const FOnStoreCartUpdate& SuccessCallback, const FOnStoreError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v1/cart/%d/item/%s"), Cart.cart_id, *ItemSKU);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("DELETE"));

	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreController::RemoveFromCart_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();

	// Try to update item quantity
	auto CartItem = Cart.Items.FindByPredicate([ItemSKU](const FStoreItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	if (CartItem)
	{
		Cart.Items.Remove(*CartItem);
	}

	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreController::UpdateVirtualItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreUpdate SuccessCallback, FOnStoreError ErrorCallback)
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
			ItemsData.Groups.Add(ItemGroup);
		}
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreController::FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchTokenSuccess SuccessCallback, FOnStoreError ErrorCallback)
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
	SuccessCallback.ExecuteIfBound(AccessToken);
}

void UXsollaStoreController::CreateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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

	Cart = FStoreCart(JsonObject->GetNumberField(TEXT("id")));
	OnCartUpdate.Broadcast(Cart);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreController::ClearCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreController::UpdateCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
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
}

void UXsollaStoreController::AddToCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaStoreController::RemoveFromCart_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreCartUpdate SuccessCallback, FOnStoreError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaStore, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

bool UXsollaStoreController::HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnStoreError ErrorCallback)
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

void UXsollaStoreController::LoadData()
{
	Cart.cart_id = UXsollaStoreSave::Load();
	OnCartUpdate.Broadcast(Cart);
}

void UXsollaStoreController::SaveData()
{
	UXsollaStoreSave::Save(Cart.cart_id);
}

TSharedRef<IHttpRequest> UXsollaStoreController::CreateHttpRequest(const FString& Url)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	// Temporal solution with headers processing on server-side #37
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=store&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_STORE_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("engine"), TEXT("ue4"));
	HttpRequest->SetHeader(TEXT("engine_v"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("sdk"), TEXT("store"));
	HttpRequest->SetHeader(TEXT("sdk_v"), XSOLLA_STORE_VERSION);

	return HttpRequest;
}

TArray<FStoreItem> UXsollaStoreController::GetVirtualItems(const FString& GroupFilter) const
{
	if (GroupFilter.IsEmpty())
	{
		return ItemsData.Items;
	}
	else
	{
		return std::move(ItemsData.Items.FilterByPredicate([GroupFilter](const FStoreItem& InStoreItem) {
			return InStoreItem.groups.Contains(GroupFilter);
		}));
	}
}

FStoreItemsData UXsollaStoreController::GetItemsData() const
{
	return ItemsData;
}

FStoreCart UXsollaStoreController::GetCart() const
{
	return Cart;
}

UXsollaStoreImageLoader* UXsollaStoreController::GetImageLoader() const
{
	return ImageLoader;
}

#undef LOCTEXT_NAMESPACE
