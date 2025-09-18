// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySubsystem.h"

#include "XsollaInventoryDataModel.h"
#include "XsollaInventoryDefines.h"
#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsUrlBuilder.h"
#include "XsollaUtilsLoggingHelper.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"
#include "XsollaLoginSubsystem.h"
#include "Engine/GameInstance.h"

#define LOCTEXT_NAMESPACE "FXsollaInventoryModule"

UXsollaInventorySubsystem::UXsollaInventorySubsystem()
	: UGameInstanceSubsystem()
{
}

void UXsollaInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize subsystem with project identifier provided by user
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	Initialize(Settings->ProjectID);

	LoginSubsystem = GetGameInstance()->GetSubsystem<UXsollaLoginSubsystem>();

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

void UXsollaInventorySubsystem::GetInventory(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
	const FOnInventoryUpdate& SuccessCallback, const FOnError& ErrorCallback,
	const int Limit, const int Offset)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Getting user inventory items"), *VA_FUNC_LINE);

	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::EnumToString<EXsollaPublishingPlatform>(Platform);
	if (!PlatformName.IsEmpty())
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Platform filter: %s"), *VA_FUNC_LINE, *PlatformName);
	}

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/inventory/items"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddNumberQueryParam(TEXT("offset"), Offset)
							.AddNumberQueryParam(TEXT("limit"), Limit)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

	XsollaUtilsLoggingHelper::LogUrl(Url, TEXT("Getting inventory items"));

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, Platform, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::GetInventory_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaInventorySubsystem::GetVirtualCurrencyBalance(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
	const FOnCurrencyBalanceUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Getting user virtual currency balance"), *VA_FUNC_LINE);

	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::EnumToString<EXsollaPublishingPlatform>(Platform);
	if (!PlatformName.IsEmpty())
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Platform filter: %s"), *VA_FUNC_LINE, *PlatformName);
	}

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/virtual_currency_balance"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

	XsollaUtilsLoggingHelper::LogUrl(Url, TEXT("Getting virtual currency balance"));

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::GetVirtualCurrencyBalance_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaInventorySubsystem::GetTimeLimitedItems(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
	const FOnTimeLimitedItemsUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Getting user time limited items"), *VA_FUNC_LINE);

	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::EnumToString<EXsollaPublishingPlatform>(Platform);
	if (!PlatformName.IsEmpty())
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Platform filter: %s"), *VA_FUNC_LINE, *PlatformName);
	}

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/time_limited_items"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

	XsollaUtilsLoggingHelper::LogUrl(Url, TEXT("Getting time limited items"));

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::GetTimeLimitedItems_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaInventorySubsystem::ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU,
	const int32 Quantity, const FString& InstanceID, const EXsollaPublishingPlatform Platform,
	const FOnInventoryRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Consuming inventory item SKU: %s"), *VA_FUNC_LINE, *ItemSKU);

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("sku"), ItemSKU);

	if (Quantity == 0)
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Quantity not specified"), *VA_FUNC_LINE);
		RequestDataJson->SetObjectField(TEXT("quantity"), nullptr);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Quantity: %d"), *VA_FUNC_LINE, Quantity);
		RequestDataJson->SetNumberField(TEXT("quantity"), Quantity);
	}

	if (InstanceID.IsEmpty())
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Instance ID not specified"), *VA_FUNC_LINE);
		RequestDataJson->SetObjectField(TEXT("instance_id"), nullptr);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Instance ID: %s"), *VA_FUNC_LINE, *InstanceID);
		RequestDataJson->SetStringField(TEXT("instance_id"), InstanceID);
	}

	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::EnumToString<EXsollaPublishingPlatform>(Platform);
	if (!PlatformName.IsEmpty())
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: Platform filter: %s"), *VA_FUNC_LINE, *PlatformName);
	}

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/inventory/item/consume"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

	XsollaUtilsLoggingHelper::LogUrl(Url, TEXT("Consuming inventory item"));

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, RequestDataJson, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, Token, SerializeJson(RequestDataJson));
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaInventorySubsystem::GetCouponRewards(const FString& AuthToken, const FString& CouponCode,
	const FOnCouponRewardsUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Getting coupon rewards for code: %s"), *VA_FUNC_LINE, *CouponCode);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/coupon/code/{CouponCode}/rewards"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.SetPathParam(TEXT("CouponCode"), CouponCode)
							.Build();

	XsollaUtilsLoggingHelper::LogUrl(Url, TEXT("Getting coupon rewards"));

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaInventorySubsystem::RedeemCoupon(const FString& AuthToken, const FString& CouponCode,
	const FOnCouponRedeemUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Redeeming coupon code: %s"), *VA_FUNC_LINE, *CouponCode);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/coupon/redeem"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.Build();

	XsollaUtilsLoggingHelper::LogUrl(Url, TEXT("Redeeming coupon"));

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("coupon_code"), CouponCode);

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, RequestDataJson, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, Token, SerializeJson(RequestDataJson));
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

bool UXsollaInventorySubsystem::IsItemInInventory(const FInventoryItemsData& Inventory, const FString& ItemSKU, FInventoryItem& FoundItem)
{
	auto FoundItemPtr = Inventory.Items.FindByPredicate([ItemSKU](const FInventoryItem& InItem)
		{ return InItem.sku == ItemSKU; });

	if (FoundItemPtr != nullptr)
	{
		FoundItem = *FoundItemPtr;
	}

	return FoundItemPtr != nullptr;
}

void UXsollaInventorySubsystem::GetInventory_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	// Log HTTP response
	XsollaUtilsLoggingHelper::LogHttpResponse(HttpRequest, HttpResponse, LogXsollaInventory);

	XsollaHttpRequestError OutError;
	FInventoryItemsData Inventory;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryItemsData::StaticStruct(), &Inventory, OutError))
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: GetInventory request successful. Items count: %d."), *VA_FUNC_LINE, Inventory.Items.Num());
		SuccessCallback.ExecuteIfBound(Inventory);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: GetInventory request failed. Error: %s"), *VA_FUNC_LINE, *OutError.description);
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::GetVirtualCurrencyBalance_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCurrencyBalanceUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	// Log HTTP response
	XsollaUtilsLoggingHelper::LogHttpResponse(HttpRequest, HttpResponse, LogXsollaInventory);

	XsollaHttpRequestError OutError;
	FVirtualCurrencyBalanceData VirtualCurrencyBalance;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrencyBalanceData::StaticStruct(), &VirtualCurrencyBalance, OutError))
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: GetVirtualCurrencyBalance request successful. Items count: %d."), *VA_FUNC_LINE, VirtualCurrencyBalance.Items.Num());
		SuccessCallback.ExecuteIfBound(VirtualCurrencyBalance);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: GetVirtualCurrencyBalance request failed. Error: %s"), *VA_FUNC_LINE, *OutError.description);
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::GetTimeLimitedItems_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnTimeLimitedItemsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	// Log HTTP response
	XsollaUtilsLoggingHelper::LogHttpResponse(HttpRequest, HttpResponse, LogXsollaInventory);

	XsollaHttpRequestError OutError;
	FTimeLimitedItemsData receivedTimeLimitedItems;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FTimeLimitedItemsData::StaticStruct(), &receivedTimeLimitedItems, OutError))
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: GetTimeLimitedItems request successful. Items count: %d."), *VA_FUNC_LINE, receivedTimeLimitedItems.Items.Num());
		SuccessCallback.ExecuteIfBound(receivedTimeLimitedItems);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: GetTimeLimitedItems request failed. Error: %s"), *VA_FUNC_LINE, *OutError.description);
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnInventoryRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	// Log HTTP response
	XsollaUtilsLoggingHelper::LogHttpResponse(HttpRequest, HttpResponse, LogXsollaInventory);

	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: ConsumeInventoryItem request successful"), *VA_FUNC_LINE);
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: ConsumeInventoryItem request failed. Error: %s"), *VA_FUNC_LINE, *OutError.description);
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCouponRewardsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	// Log HTTP response
	XsollaUtilsLoggingHelper::LogHttpResponse(HttpRequest, HttpResponse, LogXsollaInventory);

	FInventoryCouponRewardData couponRewards;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryCouponRewardData::StaticStruct(), &couponRewards, OutError))
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: GetCouponRewards request successful. Bonus items: %d"),
			*VA_FUNC_LINE, couponRewards.bonus.Num());
		SuccessCallback.ExecuteIfBound(couponRewards);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: GetCouponRewards request failed. Error: %s"), *VA_FUNC_LINE, *OutError.description);
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCouponRedeemUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	// Log HTTP response
	XsollaUtilsLoggingHelper::LogHttpResponse(HttpRequest, HttpResponse, LogXsollaInventory);

	FInventoryRedeemedCouponData redeemedCouponData;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryRedeemedCouponData::StaticStruct(), &redeemedCouponData, OutError))
	{
		UE_LOG(LogXsollaInventory, Log, TEXT("%s: RedeemCoupon request successful. Items: %d"),
			*VA_FUNC_LINE, redeemedCouponData.items.Num());
		SuccessCallback.ExecuteIfBound(redeemedCouponData);
	}
	else
	{
		UE_LOG(LogXsollaInventory, Error, TEXT("%s: RedeemCoupon request failed. Error: %s"), *VA_FUNC_LINE, *OutError.description);
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaInventorySubsystem::CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb,
	const FString& AuthToken, const FString& Content)
{
	UE_LOG(LogXsollaInventory, Log, TEXT("%s: Creating HTTP request - URL: %s, Verb: %s"),
		*VA_FUNC_LINE, *Url, *XsollaUtilsLoggingHelper::VerbToString(Verb));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = XsollaUtilsHttpRequestHelper::CreateHttpRequest(Url, Verb, AuthToken, Content, TEXT("INVENTORY"), XSOLLA_INVENTORY_VERSION);

	// Log request details
	XsollaUtilsLoggingHelper::LogHttpRequest(HttpRequest, LogXsollaInventory, Content);

	return HttpRequest;
}

FString UXsollaInventorySubsystem::SerializeJson(const TSharedPtr<FJsonObject> DataJson) const
{
	FString JsonContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonContent);
	FJsonSerializer::Serialize(DataJson.ToSharedRef(), Writer);
	return JsonContent;
}

#undef LOCTEXT_NAMESPACE
