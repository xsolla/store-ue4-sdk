// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XsollaStoreDataModel.h"
#include "XsollaUtilsDataModel.h"
#include "XsollaUtilsHttpRequestHelper.h"

class FJsonObject;

struct XSOLLASTORE_API FSubscriptionContractRequestData
{
	FString Url;
	EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET;
	TSharedPtr<FJsonObject> Body;

	bool HasBody() const
	{
		return Body.IsValid();
	}
};

class XSOLLASTORE_API FXsollaSubscriptionsContractHelper
{
public:
	static FSubscriptionContractRequestData BuildGetSubscriptionsRequest(const FString& ProjectId, const FString& Locale, int32 Limit, int32 Offset);
	static FSubscriptionContractRequestData BuildGetSubscriptionDetailsRequest(const FString& ProjectId, int32 SubscriptionId, const FString& Locale);
	static FSubscriptionContractRequestData BuildCancelSubscriptionRequest(const FString& ProjectId, int32 SubscriptionId);
	static FSubscriptionContractRequestData BuildSubscriptionRenewalRequest(const FString& ProjectId, int32 SubscriptionId,
		const FXsollaParameters& CustomParameters, const FString& SteamUserId, const TSharedPtr<FJsonObject>& PaystationSettings, bool bIsSandboxEnabled);
	static FSubscriptionContractRequestData BuildSubscriptionPurchaseRequest(const FString& ProjectId, const FString& PlanExternalId, const FString& Country,
		int32 ProductId, const TArray<FString>& AvailablePlans, const FXsollaParameters& CustomParameters,
		const TSharedPtr<FJsonObject>& PaystationSettings, bool bIsSandboxEnabled);
	static FSubscriptionContractRequestData BuildSubscriptionManagementRequest(const FString& ProjectId, const FString& Country,
		int32 ProductId, const TArray<FString>& AvailablePlans, const FXsollaParameters& CustomParameters, const FString& SteamUserId,
		const TSharedPtr<FJsonObject>& PaystationSettings, bool bIsSandboxEnabled);
	static FSubscriptionContractRequestData BuildSubscriptionUserAccountRequest(const FString& ProjectId, const FString& Locale);

	static void ApplyNullableFieldsToSubscriptionPlansList(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionPlansList& SubscriptionPlansList);
	static void ApplyNullableFieldsToSubscriptionsList(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionsList& SubscriptionsList);
	static void ApplyNullableFieldsToSubscriptionDetails(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionDetails& SubscriptionDetails);
};
