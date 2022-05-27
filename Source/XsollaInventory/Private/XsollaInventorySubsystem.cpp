// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaInventorySubsystem.h"

#include "XsollaInventoryDataModel.h"
#include "XsollaInventoryDefines.h"
#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsUrlBuilder.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"
#include "XsollaLoginSubsystem.h"

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
	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT(""):UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPublishingPlatform", Platform);
	
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/inventory/items"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddNumberQueryParam(TEXT("offset"), Offset)
							.AddNumberQueryParam(TEXT("limit"), Limit)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Platform, SuccessCallback, ErrorCallback](const FString& Token, bool bRepeatOnError)
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
	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPublishingPlatform", Platform);
	
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/virtual_currency_balance"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

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

void UXsollaInventorySubsystem::GetSubscriptions(const FString& AuthToken, const EXsollaPublishingPlatform Platform,
	const FOnSubscriptionUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPublishingPlatform", Platform);
	
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/subscriptions"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this,
			&UXsollaInventorySubsystem::GetSubscriptions_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaInventorySubsystem::ConsumeInventoryItem(const FString& AuthToken, const FString& ItemSKU,
	const int32 Quantity, const FString& InstanceID, const EXsollaPublishingPlatform Platform,
	const FOnInventoryRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
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

	const FString PlatformName = Platform == EXsollaPublishingPlatform::undefined ? TEXT("") : UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaPublishingPlatform", Platform);
	
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/user/inventory/item/consume"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.AddStringQueryParam(TEXT("platform"), PlatformName)
							.Build();

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
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/coupon/code/{CouponCode}/rewards"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.SetPathParam(TEXT("CouponCode"), CouponCode)
							.Build();

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
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://store.xsolla.com/api/v2/project/{ProjectID}/coupon/redeem"))
							.SetPathParam(TEXT("ProjectID"), ProjectID)
							.Build();

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

void UXsollaInventorySubsystem::GetInventory_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnInventoryUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FInventoryItemsData NewInventory;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryItemsData::StaticStruct(), &NewInventory, OutError))
	{
		SuccessCallback.ExecuteIfBound(NewInventory);
	}
	else
	{
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::GetVirtualCurrencyBalance_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCurrencyBalanceUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FVirtualCurrencyBalanceData VirtualCurrencyBalance;
	
	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FVirtualCurrencyBalanceData::StaticStruct(), &VirtualCurrencyBalance, OutError))
	{
		SuccessCallback.ExecuteIfBound(VirtualCurrencyBalance);
	}
	else
	{
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::GetSubscriptions_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnSubscriptionUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FSubscriptionData receivedSubscriptions;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FSubscriptionData::StaticStruct(), &receivedSubscriptions, OutError))
	{
		SuccessCallback.ExecuteIfBound(receivedSubscriptions);
	}
	else
	{
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::ConsumeInventoryItem_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnInventoryRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::UpdateCouponRewards_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCouponRewardsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	FInventoryCouponRewardData couponRewards;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryCouponRewardData::StaticStruct(), &couponRewards, OutError))
	{
		SuccessCallback.ExecuteIfBound(couponRewards);
	}
	else
	{
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaInventorySubsystem::RedeemCoupon_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnCouponRedeemUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	FInventoryRedeemedCouponData redeemedCouponData;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FInventoryRedeemedCouponData::StaticStruct(), &redeemedCouponData, OutError))
	{
		SuccessCallback.ExecuteIfBound(redeemedCouponData);
	}
	else
	{
		LoginSubsystem->HandleRequestError(OutError, ErrorHandlersWrapper);
	}
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

#undef LOCTEXT_NAMESPACE
