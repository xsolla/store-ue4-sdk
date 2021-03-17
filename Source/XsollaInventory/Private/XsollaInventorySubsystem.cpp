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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken, SerializeJson(RequestDataJson));
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
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

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::UpdateInventory_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FInventoryItemsData NewInventory;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryItemsData::StaticStruct(), &NewInventory, OutError))
	{
		Inventory = NewInventory;
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaInventorySubsystem::UpdateVirtualCurrencyBalance_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrencyBalanceData::StaticStruct(), &VirtualCurrencyBalance, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaInventorySubsystem::UpdateSubscriptions_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FSubscriptionData receivedSubscriptions;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FSubscriptionData::StaticStruct(), &receivedSubscriptions, OutError))
	{
		Subscriptions = receivedSubscriptions;
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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
}

void UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCouponRewardsUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	FInventoryCouponRewardData couponRewards;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryCouponRewardData::StaticStruct(), &couponRewards, OutError))
	{
		SuccessCallback.ExecuteIfBound(couponRewards);
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCouponRedeemUpdate SuccessCallback, FOnInventoryError ErrorCallback)
{
	FInventoryRedeemedCouponData redeemedCouponData;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryRedeemedCouponData::StaticStruct(), &redeemedCouponData, OutError))
	{
		SuccessCallback.ExecuteIfBound(redeemedCouponData);
	}
	else
	{
		HandleRequestError(OutError, ErrorCallback);
	}
}

void UXsollaInventorySubsystem::HandleRequestError(XsollaHttpRequestError ErrorData, FOnInventoryError ErrorCallback)
{
	UE_LOG(LogXsollaInventory, Error, TEXT("%s: request failed - Status code: %d, Error code: %d, Error message: %s"), *VA_FUNC_LINE, ErrorData.statusCode, ErrorData.errorCode, *ErrorData.errorMessage);
	ErrorCallback.ExecuteIfBound(ErrorData.statusCode, ErrorData.errorCode, ErrorData.errorMessage);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaInventorySubsystem::CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb,
	const FString& AuthToken, const FString& Content)
{
	return XsollaUtilsHttpRequestHelper::CreateHttpRequest(Url, Verb, AuthToken, Content, TEXT("INVENTORY"), XSOLLA_INVENTORY_VERSION);
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
