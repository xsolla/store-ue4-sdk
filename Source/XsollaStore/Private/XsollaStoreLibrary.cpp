// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreLibrary.h"

#include "XsollaStore.h"

#include "XsollaStoreDataModel.h"

UXsollaStoreLibrary::UXsollaStoreLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaStoreSettings* UXsollaStoreLibrary::GetStoreSettings()
{
	return FXsollaStoreModule::Get().GetSettings();
}

bool UXsollaStoreLibrary::Equal_StoreCartStoreCart(const FStoreCart& A, const FStoreCart& B)
{
	return A == B;
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_IntToXsollaJsonVariant(int Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_FloatToXsollaJsonVariant(float Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_StringToXsollaJsonVariant(const FString& Value)
{
	return FXsollaJsonVariant(Value);
}

FXsollaJsonVariant UXsollaStoreLibrary::Conv_BoolToXsollaJsonVariant(bool Value)
{
	return FXsollaJsonVariant(Value);
}

void UXsollaStoreLibrary::AddCustomParameters(TSharedPtr<FJsonObject> JsonObject, FXsollaPaymentCustomParameters CustomParameters)
{
	if (CustomParameters.Parameters.Num() == 0)
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObjectCustomParameters = MakeShareable(new FJsonObject());

	for (auto Parameter : CustomParameters.Parameters)
	{
		const FVariant Variant = Parameter.Value.GetVariant();
		
		switch (Variant.GetType())
		{
		case EVariantTypes::Bool:
			JsonObjectCustomParameters->SetBoolField(Parameter.Key, Variant.GetValue<bool>());
			break;
		case EVariantTypes::String:
			JsonObjectCustomParameters->SetStringField(Parameter.Key, Variant.GetValue<FString>());
			break;
		case EVariantTypes::Int32:
			JsonObjectCustomParameters->SetNumberField(Parameter.Key, Variant.GetValue<int>());
			break;
		case EVariantTypes::Float:
			JsonObjectCustomParameters->SetNumberField(Parameter.Key, Variant.GetValue<float>());
			break;
		default:
			UE_LOG(LogXsollaStore, Log, TEXT("%s: parameter with type of %d was not added"), *VA_FUNC_LINE, Variant.GetType());
			break;
		}
	}

	JsonObject->SetObjectField("custom_parameters", JsonObjectCustomParameters);
}
