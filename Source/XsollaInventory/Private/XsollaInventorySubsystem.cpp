// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySubsystem.h"

#include "XsollaInventory.h"
#include "XsollaInventoryDataModel.h"
#include "XsollaInventoryDefines.h"
#include "XsollaInventorySettings.h"
#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsUrlBuilder.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

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
	const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback, const int Limit, const int Offset)
{
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/inventory/items"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddNumberQueryParam(TEXT("offset"), Offset)
							.AddNumberQueryParam(TEXT("limit"), Limit)
							.AddStringQueryParam(TEXT("platform"), GetPublishingPlatformName())
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateInventory_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::UpdateVirtualCurrencyBalance(const FString& AuthToken, const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/virtual_currency_balance"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), GetPublishingPlatformName())
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateVirtualCurrencyBalance_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::UpdateSubscriptions(const FString& AuthToken,
	const FOnInventoryUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/subscriptions"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), GetPublishingPlatformName())
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateSubscriptions_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU,
	const int32 Quantity, const FString& InstanceID,
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

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/inventory/item/consume"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), GetPublishingPlatformName())
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, AuthToken, SerializeJson(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete, SuccessCallback, ErrorCallback);

	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::GetCouponRewards(const FString& AuthToken, const FString& CouponCode,
	const FOnCouponRewardsUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/coupon/code/{CouponCode}/rewards"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.SetPathParam(TEXT("CouponCode"), CouponCode)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaInventorySubsystem::RedeemCoupon(const FString& AuthToken, const FString& CouponCode,
	const FOnCouponRedeemUpdate& SuccessCallback, const FOnInventoryError& ErrorCallback)
{
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/coupon/redeem"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.Build();

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
	const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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
	const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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
	const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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
	const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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

void UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCouponRewardsUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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

void UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCouponRedeemUpdate SuccessCallback, FOnInventoryError ErrorCallback)
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

void UXsollaInventorySubsystem::HandleRequestError(const XsollaHttpRequestError& ErrorData, FOnInventoryError ErrorCallback)
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

FString UXsollaInventorySubsystem::GetPublishingPlatformName() const
{
	const UXsollaInventorySettings* Settings = FXsollaInventoryModule::Get().GetSettings();

	if (!Settings->UseCrossPlatformAccountLinking)
	{
		return TEXT("");
	}

	return UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPublishingPlatform", Settings->Platform);
}

FInventoryItemsData UXsollaInventorySubsystem::GetInventory() const
{
	return Inventory;
}

TArray<FVirtualCurrencyBalance> UXsollaInventorySubsystem::GetVirtualCurrencyBalance() const
{
	return VirtualCurrencyBalance.Items;
}

FVirtualCurrencyBalance UXsollaInventorySubsystem::GetVirtualCurrencyBalanceBySku(const FString& CurrencySku, bool& bWasFound) const
{
	FVirtualCurrencyBalance Currency;
	bWasFound = false;

	const auto InventoryItem =
		VirtualCurrencyBalance.Items.FindByPredicate(
			[CurrencySku](const FVirtualCurrencyBalance& InCurrency) {
				return InCurrency.sku == CurrencySku;
			});

	if (InventoryItem != nullptr)
	{
		bWasFound = true;
		Currency = InventoryItem[0];
	}

	return Currency;
}

TArray<FSubscriptionItem> UXsollaInventorySubsystem::GetSubscriptions() const
{
	return Subscriptions.Items;
}

FString UXsollaInventorySubsystem::GetItemName(const FString& ItemSKU) const
{
	auto InventoryItem = Inventory.Items.FindByPredicate([ItemSKU](const FInventoryItem& InItem) {
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
	auto InventoryItem = Inventory.Items.FindByPredicate([ItemSKU](const FInventoryItem& InItem) {
		return InItem.sku == ItemSKU;
	});

	return InventoryItem != nullptr;
}

#undef LOCTEXT_NAMESPACE
