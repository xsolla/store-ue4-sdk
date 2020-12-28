// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsDefines.h"
#include "XsollaUtilsModule.h"

#include "Dom/JsonObject.h"

UXsollaUtilsLibrary::UXsollaUtilsLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXsollaUtilsLibrary::Internal_AddParametersToJsonObject(TSharedPtr<FJsonObject> JsonObject,
	FXsollaParameters CustomParameters, const FString& FieldName)
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

	for (auto Parameter : CustomParameters.Parameters)
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

FDateTime UXsollaUtilsLibrary::MakeDateTimeFromTimestamp(const int64 Time)
{
	return FDateTime::FromUnixTimestamp(Time);
}

int64 UXsollaUtilsLibrary::GetSecondsFromUnixTimestamp(const FDateTime DateTime)
{
	return DateTime.ToUnixTimestamp();
}

void UXsollaUtilsLibrary::AddParametersToJsonObject(TSharedPtr<FJsonObject> JsonObject, const FXsollaParameters CustomParameters)
{
	Internal_AddParametersToJsonObject(JsonObject, CustomParameters);
}

void UXsollaUtilsLibrary::AddParametersToJsonObjectByFieldName(TSharedPtr<FJsonObject> JsonObject, const FString& FieldName,
	const FXsollaParameters CustomParameters)
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

FXsollaJsonVariant UXsollaUtilsLibrary::Conv_StringToXsollaJsonVariant(const FString& Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaUtilsLibrary::Conv_BoolToXsollaJsonVariant(bool Value)
{
	return FXsollaJsonVariant(Value);
}