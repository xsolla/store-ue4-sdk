// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaSubscriptionsContractHelper.h"

#include "Dom/JsonObject.h"
#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsUrlBuilder.h"

namespace
{
	bool HasNonNullField(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName)
	{
		if (!JsonObject.IsValid() || !JsonObject->HasField(FieldName))
		{
			return false;
		}

		const TSharedPtr<FJsonValue>* FieldValue = JsonObject->Values.Find(FieldName);
		return FieldValue && FieldValue->IsValid() && (*FieldValue)->Type != EJson::Null;
	}

	void UpdateNullablePeriodFields(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionPeriod& Period)
	{
		Period.has_value = HasNonNullField(JsonObject, TEXT("value"));
	}

	void UpdateNullableChargeFields(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionCharge& Charge)
	{
		Charge.has_amount_with_promotion = HasNonNullField(JsonObject, TEXT("amount_with_promotion"));
	}

	void UpdateNullablePlanPromotionFields(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionPlanPromotion& Promotion)
	{
		Promotion.has_promotion_charge_amount = HasNonNullField(JsonObject, TEXT("promotion_charge_amount"));
		Promotion.has_promotion_remaining_charges = HasNonNullField(JsonObject, TEXT("promotion_remaining_charges"));
	}

	void UpdateNullableSubscriptionPlanFields(const TSharedPtr<FJsonObject>& JsonObject, FSubscriptionPlan& SubscriptionPlan)
	{
		SubscriptionPlan.has_plan_start_date = HasNonNullField(JsonObject, TEXT("plan_start_date"));
		SubscriptionPlan.has_plan_end_date = HasNonNullField(JsonObject, TEXT("plan_end_date"));
		SubscriptionPlan.has_trial_period = HasNonNullField(JsonObject, TEXT("trial_period"));

		if (JsonObject.IsValid() && JsonObject->HasTypedField<EJson::Object>(TEXT("period")))
		{
			UpdateNullablePeriodFields(JsonObject->GetObjectField(TEXT("period")), SubscriptionPlan.period);
		}

		if (JsonObject.IsValid() && JsonObject->HasTypedField<EJson::Object>(TEXT("promotion")))
		{
			UpdateNullablePlanPromotionFields(JsonObject->GetObjectField(TEXT("promotion")), SubscriptionPlan.promotion);
		}
	}

	void UpdateNullableSubscriptionFields(const TSharedPtr<FJsonObject>& JsonObject, FSubscription& Subscription)
	{
		Subscription.has_plan_start_date = HasNonNullField(JsonObject, TEXT("plan_start_date"));
		Subscription.has_plan_end_date = HasNonNullField(JsonObject, TEXT("plan_end_date"));
		Subscription.has_product_id = HasNonNullField(JsonObject, TEXT("product_id"));
		Subscription.has_trial_period = HasNonNullField(JsonObject, TEXT("trial_period"));
		Subscription.has_date_next_charge = HasNonNullField(JsonObject, TEXT("date_next_charge"));
		Subscription.has_date_last_charge = HasNonNullField(JsonObject, TEXT("date_last_charge"));

		if (JsonObject.IsValid() && JsonObject->HasTypedField<EJson::Object>(TEXT("charge")))
		{
			UpdateNullableChargeFields(JsonObject->GetObjectField(TEXT("charge")), Subscription.charge);
		}

		if (JsonObject.IsValid() && JsonObject->HasTypedField<EJson::Object>(TEXT("period")))
		{
			UpdateNullablePeriodFields(JsonObject->GetObjectField(TEXT("period")), Subscription.period);
		}
	}

	TArray<TSharedPtr<FJsonValue>> BuildStringJsonArray(const TArray<FString>& Values)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		JsonArray.Reserve(Values.Num());
		for (const FString& Value : Values)
		{
			JsonArray.Add(MakeShared<FJsonValueString>(Value));
		}

		return JsonArray;
	}

	TSharedPtr<FJsonObject> ClonePaystationSettings(const TSharedPtr<FJsonObject>& PaystationSettings)
	{
		return PaystationSettings.IsValid()
			? MakeShared<FJsonObject>(*PaystationSettings)
			: MakeShared<FJsonObject>();
	}
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildGetSubscriptionsRequest(
	const FString& ProjectId,
	const FString& Locale,
	int32 Limit,
	int32 Offset)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_GET;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.AddStringQueryParam(TEXT("locale"), Locale)
		.AddNumberQueryParam(TEXT("limit"), Limit)
		.AddNumberQueryParam(TEXT("offset"), Offset)
		.Build();

	return Result;
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildGetSubscriptionDetailsRequest(
	const FString& ProjectId,
	int32 SubscriptionId,
	const FString& Locale)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_GET;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions/{SubscriptionId}"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.SetPathParam(TEXT("SubscriptionId"), SubscriptionId)
		.AddStringQueryParam(TEXT("locale"), Locale)
		.Build();

	return Result;
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildCancelSubscriptionRequest(const FString& ProjectId, int32 SubscriptionId)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_PUT;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions/{SubscriptionId}/cancel"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.SetPathParam(TEXT("SubscriptionId"), SubscriptionId)
		.Build();

	return Result;
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildSubscriptionRenewalRequest(
	const FString& ProjectId,
	int32 SubscriptionId,
	const FXsollaParameters& CustomParameters,
	const FString& SteamUserId,
	const TSharedPtr<FJsonObject>& PaystationSettings,
	bool bIsSandboxEnabled)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_POST;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions/{SubscriptionId}/renew"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.SetPathParam(TEXT("SubscriptionId"), SubscriptionId)
		.Build();

	Result.Body = MakeShared<FJsonObject>();
	TSharedPtr<FJsonObject> SettingsJson = ClonePaystationSettings(PaystationSettings);
	SettingsJson->SetBoolField(TEXT("sandbox"), bIsSandboxEnabled);
	if (!SteamUserId.IsEmpty())
	{
		SettingsJson->SetStringField(TEXT("steam_user_id"), SteamUserId);
	}
	Result.Body->SetObjectField(TEXT("settings"), SettingsJson);

	UXsollaUtilsLibrary::AddParametersToJsonObjectByFieldName(Result.Body, TEXT("custom_parameters"), CustomParameters);
	if (!SteamUserId.IsEmpty())
	{
		Result.Body->SetStringField(TEXT("steam_user_id"), SteamUserId);
	}

	return Result;
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildSubscriptionPurchaseRequest(
	const FString& ProjectId,
	const FString& PlanExternalId,
	const FString& Country,
	int32 ProductId,
	const TArray<FString>& AvailablePlans,
	const FXsollaParameters& CustomParameters,
	const TSharedPtr<FJsonObject>& PaystationSettings,
	bool bIsSandboxEnabled)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_POST;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions/buy"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.AddStringQueryParam(TEXT("country"), Country)
		.Build();

	Result.Body = MakeShared<FJsonObject>();
	Result.Body->SetStringField(TEXT("plan_external_id"), PlanExternalId);
	if (ProductId > 0)
	{
		Result.Body->SetNumberField(TEXT("product_id"), ProductId);
	}
	if (AvailablePlans.Num() > 0)
	{
		Result.Body->SetArrayField(TEXT("available_plans"), BuildStringJsonArray(AvailablePlans));
	}
	UXsollaUtilsLibrary::AddParametersToJsonObjectByFieldName(Result.Body, TEXT("custom_parameters"), CustomParameters);

	TSharedPtr<FJsonObject> SettingsJson = ClonePaystationSettings(PaystationSettings);
	SettingsJson->SetBoolField(TEXT("sandbox"), bIsSandboxEnabled);
	Result.Body->SetObjectField(TEXT("settings"), SettingsJson);

	return Result;
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildSubscriptionManagementRequest(
	const FString& ProjectId,
	const FString& Country,
	int32 ProductId,
	const TArray<FString>& AvailablePlans,
	const FXsollaParameters& CustomParameters,
	const FString& SteamUserId,
	const TSharedPtr<FJsonObject>& PaystationSettings,
	bool bIsSandboxEnabled)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_POST;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions/manage"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.AddStringQueryParam(TEXT("country"), Country)
		.Build();

	Result.Body = MakeShared<FJsonObject>();
	if (ProductId > 0)
	{
		Result.Body->SetNumberField(TEXT("product_id"), ProductId);
	}
	if (AvailablePlans.Num() > 0)
	{
		Result.Body->SetArrayField(TEXT("available_plans"), BuildStringJsonArray(AvailablePlans));
	}
	UXsollaUtilsLibrary::AddParametersToJsonObjectByFieldName(Result.Body, TEXT("custom_parameters"), CustomParameters);
	if (!SteamUserId.IsEmpty())
	{
		Result.Body->SetStringField(TEXT("steam_user_id"), SteamUserId);
	}

	TSharedPtr<FJsonObject> SettingsJson = ClonePaystationSettings(PaystationSettings);
	SettingsJson->SetBoolField(TEXT("sandbox"), bIsSandboxEnabled);
	Result.Body->SetObjectField(TEXT("settings"), SettingsJson);

	return Result;
}

FSubscriptionContractRequestData FXsollaSubscriptionsContractHelper::BuildSubscriptionUserAccountRequest(const FString& ProjectId, const FString& Locale)
{
	FSubscriptionContractRequestData Result;
	Result.Verb = EXsollaHttpRequestVerb::VERB_GET;
	Result.Url = XsollaUtilsUrlBuilder(TEXT("https://subscriptions.xsolla.com/api/user/v1/projects/{ProjectID}/subscriptions/user_account"))
		.SetPathParam(TEXT("ProjectID"), ProjectId)
		.AddStringQueryParam(TEXT("locale"), Locale)
		.Build();

	return Result;
}

void FXsollaSubscriptionsContractHelper::ApplyNullableFieldsToSubscriptionPlansList(
	const TSharedPtr<FJsonObject>& JsonObject,
	FSubscriptionPlansList& SubscriptionPlansList)
{
	if (!JsonObject.IsValid() || !JsonObject->HasTypedField<EJson::Array>(TEXT("items")))
	{
		return;
	}

	const TArray<TSharedPtr<FJsonValue>> Items = JsonObject->GetArrayField(TEXT("items"));
	const int32 PlansCount = FMath::Min(Items.Num(), SubscriptionPlansList.items.Num());

	for (int32 i = 0; i < PlansCount; ++i)
	{
		if (Items[i].IsValid() && Items[i]->Type == EJson::Object)
		{
			UpdateNullableSubscriptionPlanFields(Items[i]->AsObject(), SubscriptionPlansList.items[i]);
		}
	}
}

void FXsollaSubscriptionsContractHelper::ApplyNullableFieldsToSubscriptionsList(
	const TSharedPtr<FJsonObject>& JsonObject,
	FSubscriptionsList& SubscriptionsList)
{
	if (!JsonObject.IsValid() || !JsonObject->HasTypedField<EJson::Array>(TEXT("items")))
	{
		return;
	}

	const TArray<TSharedPtr<FJsonValue>> Items = JsonObject->GetArrayField(TEXT("items"));
	const int32 SubscriptionsCount = FMath::Min(Items.Num(), SubscriptionsList.items.Num());

	for (int32 i = 0; i < SubscriptionsCount; ++i)
	{
		if (Items[i].IsValid() && Items[i]->Type == EJson::Object)
		{
			UpdateNullableSubscriptionFields(Items[i]->AsObject(), SubscriptionsList.items[i]);
		}
	}
}

void FXsollaSubscriptionsContractHelper::ApplyNullableFieldsToSubscriptionDetails(
	const TSharedPtr<FJsonObject>& JsonObject,
	FSubscriptionDetails& SubscriptionDetails)
{
	SubscriptionDetails.has_plan_start_date = HasNonNullField(JsonObject, TEXT("plan_start_date"));
	SubscriptionDetails.has_plan_end_date = HasNonNullField(JsonObject, TEXT("plan_end_date"));
	SubscriptionDetails.has_product_id = HasNonNullField(JsonObject, TEXT("product_id"));
	SubscriptionDetails.has_trial_period = HasNonNullField(JsonObject, TEXT("trial_period"));
	SubscriptionDetails.has_date_next_charge = HasNonNullField(JsonObject, TEXT("date_next_charge"));
	SubscriptionDetails.has_date_last_charge = HasNonNullField(JsonObject, TEXT("date_last_charge"));
	SubscriptionDetails.has_date_end = HasNonNullField(JsonObject, TEXT("date_end"));

	if (JsonObject.IsValid() && JsonObject->HasTypedField<EJson::Object>(TEXT("charge")))
	{
		UpdateNullableChargeFields(JsonObject->GetObjectField(TEXT("charge")), SubscriptionDetails.charge);
	}

	if (JsonObject.IsValid() && JsonObject->HasTypedField<EJson::Object>(TEXT("period")))
	{
		UpdateNullablePeriodFields(JsonObject->GetObjectField(TEXT("period")), SubscriptionDetails.period);
	}
}
