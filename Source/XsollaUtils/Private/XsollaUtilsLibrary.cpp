// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsDefines.h"
#include "XsollaUtilsModule.h"

#include "Dom/JsonObject.h"
#include "Interfaces/IPluginManager.h"

FString UXsollaUtilsLibrary::XReferral(TEXT(""));
FString UXsollaUtilsLibrary::XReferralVersion(TEXT(""));

UXsollaUtilsLibrary::UXsollaUtilsLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXsollaUtilsLibrary::Internal_AddParametersToJsonObject(TSharedPtr<FJsonObject> JsonObject,
	const FXsollaParameters& CustomParameters, const FString& FieldName)
{
	if (CustomParameters.Parameters.Num() == 0)
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonRequestObject;
	if (FieldName.IsEmpty())
	{
		JsonRequestObject = JsonObject;
	}
	else
	{
		JsonRequestObject = MakeShareable(new FJsonObject);
	}

	for (auto& Parameter : CustomParameters.Parameters)
	{
		const FVariant Variant = Parameter.Value.Variant;

		switch (Variant.GetType())
		{
		case EVariantTypes::Bool:
			JsonRequestObject->SetBoolField(Parameter.Key, Variant.GetValue<bool>());
			break;
		case EVariantTypes::String:
			JsonRequestObject->SetStringField(Parameter.Key, Variant.GetValue<FString>());
			break;
		case EVariantTypes::Int32:
			JsonRequestObject->SetNumberField(Parameter.Key, Variant.GetValue<int>());
			break;
		case EVariantTypes::Float:
			JsonRequestObject->SetNumberField(Parameter.Key, Variant.GetValue<float>());
			break;
		default:
			UE_LOG(LogXsollaUtils, Log, TEXT("%s: parameter with type of %s was not added"),
				*VA_FUNC_LINE, *UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaVariantTypes", static_cast<EXsollaVariantTypes>(Variant.GetType())));
			break;
		}
	}

	if (!FieldName.IsEmpty())
	{
		JsonObject->SetObjectField(FieldName, JsonRequestObject);
	}
}

UXsollaUtilsImageLoader* UXsollaUtilsLibrary::GetImageLoader()
{
	return FXsollaUtilsModule::Get().GetImageLoader();
}

void UXsollaUtilsLibrary::GetDefaultObject(TSubclassOf<UObject> ObjectClass, UObject*& DefaultObj)
{
	if (ObjectClass)
	{
		DefaultObj = ObjectClass->GetDefaultObject();
	}
}

FDateTime UXsollaUtilsLibrary::MakeDateTimeFromTimestamp(const int64 Time)
{
	return FDateTime::FromUnixTimestamp(Time);
}

int64 UXsollaUtilsLibrary::GetSecondsFromUnixTimestamp(const FDateTime& DateTime)
{
	return DateTime.ToUnixTimestamp();
}

void UXsollaUtilsLibrary::AddParametersToJsonObject(TSharedPtr<FJsonObject> JsonObject, const FXsollaParameters& CustomParameters)
{
	Internal_AddParametersToJsonObject(JsonObject, CustomParameters);
}

void UXsollaUtilsLibrary::AddParametersToJsonObjectByFieldName(TSharedPtr<FJsonObject> JsonObject, const FString& FieldName,
	const FXsollaParameters& CustomParameters)
{
	Internal_AddParametersToJsonObject(JsonObject, CustomParameters, FieldName);
}

FXsollaJsonVariant UXsollaUtilsLibrary::Conv_IntToXsollaJsonVariant(int Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaUtilsLibrary::Conv_FloatToXsollaJsonVariant(float Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaUtilsLibrary::Conv_StringToXsollaJsonVariant(FString Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaUtilsLibrary::Conv_BoolToXsollaJsonVariant(bool Value)
{
	return FXsollaJsonVariant(Value);
}

void UXsollaUtilsLibrary::SetPartnerInfo(const FString& Referral, const FString& ReferralVersion)
{
	XReferral = Referral;
	XReferralVersion = ReferralVersion;
}

void UXsollaUtilsLibrary::GetPartnerInfo(FString& Referral, FString& ReferralVersion)
{
	Referral = XReferral;
	ReferralVersion = XReferralVersion;
}

FString UXsollaUtilsLibrary::EncodeFormData(TSharedPtr<FJsonObject> FormDataJson)
{
	FString EncodedFormData = "";
	uint16 ParamIndex = 0;

	for (auto FormDataIt = FormDataJson->Values.CreateIterator(); FormDataIt; ++FormDataIt)
	{
		FString Key = FormDataIt.Key();
		FString Value = FormDataIt.Value().Get()->AsString();

		if (!Key.IsEmpty() && !Value.IsEmpty())
		{
			EncodedFormData += ParamIndex == 0 ? "" : "&";
			EncodedFormData += FGenericPlatformHttp::UrlEncode(Key) + "=" + FGenericPlatformHttp::UrlEncode(Value);
		}

		ParamIndex++;
	}

	return EncodedFormData;
}

FString UXsollaUtilsLibrary::GetPluginName(const FName& ModuleName)
{
	for (const auto& Plugin : IPluginManager::Get().GetEnabledPlugins())
	{
		for (const auto& PluginModule : Plugin->GetDescriptor().Modules)
		{
			if (PluginModule.Name == ModuleName)
			{
				return Plugin->GetName();
			}
		}
	}

	return TEXT("");
}

FString UXsollaUtilsLibrary::GetUrlParameter(const FString& Url, const FString& ParamName)
{
	const FString DecodedUrl = FGenericPlatformHttp::UrlDecode(Url);
	const FString UrlOptions = DecodedUrl.RightChop(DecodedUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));

	return UGameplayStatics::ParseOption(UrlOptions, ParamName);
}
