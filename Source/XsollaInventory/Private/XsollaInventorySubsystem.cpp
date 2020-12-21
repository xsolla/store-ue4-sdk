// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySubsystem.h"

#include "XsollaInventory.h"
#include "XsollaInventoryDataModel.h"
#include "XsollaInventoryDefines.h"
#include "XsollaInventorySettings.h"

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

#define LOCTEXT_NAMESPACE "FXsollaInventoryModule"

UXsollaInventorySubsystem::UXsollaInventorySubsystem()
	: UGameInstanceSubsystem()
{
}

void UXsollaInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize subsystem with project identifier provided by user
	const UXsollaInventorySettings* Settings = FXsollaInventoryModule::Get().GetSettings();
	Initialize(Settings->ProjectID);

	UE_LOG(LogXsollaInventory, Log, TEXT("%s: XsollaInventory subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaInventorySubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaInventorySubsystem::Initialize(const FString& InProjectId)
{
	ProjectID = InProjectId;
}

void UXsollaInventorySubsystem::UpdateInventory(const FString& AuthToken,
	const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/inventory/items"),
		*ProjectID);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaInventoryRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateInventory_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::UpdateVirtualCurrencyBalance(const FString& AuthToken, const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/virtual_currency_balance"),
		*ProjectID);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaInventoryRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateVirtualCurrencyBalance_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::UpdateSubscriptions(const FString& AuthToken,
	const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/subscriptions"),
		*ProjectID);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaInventoryRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateSubscriptions_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU,
	int32 Quantity, const FString& InstanceID,
	const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
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

	FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/user/inventory/item/consume"),
		*ProjectID);

	const FString Platform = GetPublishingPlatformName();
	if (!Platform.IsEmpty())
	{
		Url += FString::Printf(TEXT("%splatform=%s"), Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"), *Platform);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaInventoryRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete, SuccessCallback, ErrorCallback);

	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::GetCouponRewards(const FString& AuthToken, const FString& CouponCode,
	const FOnCouponRewardsUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/coupon/code/%s/rewards"),
		*ProjectID,
		*CouponCode);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaInventoryRequestVerb::GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::RedeemCoupon(const FString& AuthToken, const FString& CouponCode, const FOnCouponRedeemUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://store.xsolla.com/api/v2/project/%s/coupon/redeem"), *ProjectID);

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("coupon_code"), CouponCode);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaInventoryRequestVerb::POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::UpdateInventory_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FInventoryItemsData NewInventory;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FInventoryItemsData::StaticStruct(), &NewInventory))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	Inventory = NewInventory;

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaInventory, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaInventorySubsystem::UpdateVirtualCurrencyBalance_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FVirtualCurrencyBalanceData::StaticStruct(), &VirtualCurrencyBalance))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaInventory, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaInventorySubsystem::UpdateSubscriptions_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FSubscriptionData receivedSubscriptions;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FSubscriptionData::StaticStruct(), &receivedSubscriptions))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	Subscriptions = receivedSubscriptions;

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaInventory, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaInventory, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCouponRewardsUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FInventoryCouponRewardData couponRewards;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FInventoryCouponRewardData::StaticStruct(), &couponRewards))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaInventory, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound(couponRewards);
}

void UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCouponRedeemUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't deserialize server response"));
		return;
	}

	FInventoryRedeemedCouponData redeemedCouponData;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FInventoryRedeemedCouponData::StaticStruct(), &redeemedCouponData))
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), 0, TEXT("Can't convert server response to struct"));
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaInventory, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound(redeemedCouponData);
}

bool UXsollaInventorySubsystem::HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryError ErrorCallback)
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
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
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
		UE_LOG(LogXsollaInventory, Warning, TEXT("%s: request failed (%s): %s"), *VA_FUNC_LINE, *ErrorStr, *ResponseStr);
		ErrorCallback.ExecuteIfBound(StatusCode, ErrorCode, ErrorStr);
		return true;
	}

	return false;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaInventorySubsystem::CreateHttpRequest(const FString& Url, const EXsollaInventoryRequestVerb Verb,
	const FString& AuthToken, const FString& Content)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Temporal solution with headers processing on server-side #37
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=inventory&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_INVENTORY_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("INVENTORY"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_INVENTORY_VERSION);

	switch (Verb)
	{
	case EXsollaInventoryRequestVerb::GET: HttpRequest->SetVerb(TEXT("GET"));

		// Check that we doen't provide content with GET request
		if (!Content.IsEmpty())
		{
			UE_LOG(LogXsollaInventory, Warning, TEXT("%s: Request content is not empty for GET request. Maybe you should use POST one?"), *VA_FUNC_LINE);
		}
		break;

	case EXsollaInventoryRequestVerb::POST: HttpRequest->SetVerb(TEXT("POST"));
		break;

	case EXsollaInventoryRequestVerb::PUT: HttpRequest->SetVerb(TEXT("PUT"));
		break;

	case EXsollaInventoryRequestVerb::DELETE: HttpRequest->SetVerb(TEXT("DELETE"));
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

FString UXsollaInventorySubsystem::SerializeJson(const TSharedPtr<FJsonObject> DataJson) const
{
	FString JsonContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonContent);
	FJsonSerializer::Serialize(DataJson.ToSharedRef(), Writer);
	return JsonContent;
}

FString UXsollaInventorySubsystem::GetPublishingPlatformName()
{
	const UXsollaInventorySettings* Settings = FXsollaInventoryModule::Get().GetSettings();

	FString platform;

	if (!Settings->UseCrossPlatformAccountLinking)
	{
		return platform;
	}

	switch (Settings->Platform)
	{
	case EXsollaPublishingPlatform::PlaystationNetwork: platform = TEXT("playstation_network");
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

FInventoryItemsData UXsollaInventorySubsystem::GetInventory() const
{
	return Inventory;
}

TArray<FVirtualCurrencyBalance> UXsollaInventorySubsystem::GetVirtualCurrencyBalance() const
{
	return VirtualCurrencyBalance.Items;
}

TArray<FSubscriptionItem> UXsollaInventorySubsystem::GetSubscriptions() const
{
	return Subscriptions.Items;
}

FString UXsollaInventorySubsystem::GetItemName(const FString& ItemSKU) const
{
	auto InventoryItem = Inventory.Items.FindByPredicate([ItemSKU](const FInventoryItem& InItem)
	{
		return InItem.sku == ItemSKU;
	});

	if (InventoryItem != nullptr)
	{
		return InventoryItem->name;
	}

	return TEXT("");
}

bool UXsollaInventorySubsystem::IsItemInInventory(const FString& ItemSKU) const
{
	auto InventoryItem = Inventory.Items.FindByPredicate([ItemSKU](const FInventoryItem& InItem)
	{
		return InItem.sku == ItemSKU;
	});

	return InventoryItem != nullptr;
}

#undef LOCTEXT_NAMESPACE
