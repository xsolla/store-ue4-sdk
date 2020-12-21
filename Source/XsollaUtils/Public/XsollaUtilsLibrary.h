// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "Components/CheckBox.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaUtilsLibrary.generated.h"

class UXsollaUtilsImageLoader;

UCLASS()
class XSOLLAUTILS_API UXsollaUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to the image loader object. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Utils")
	static UXsollaUtilsImageLoader* GetImageLoader();

	/** Make FDateTime structure based on a given timestamp. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Utils")
	static FDateTime MakeDateTimeFromTimestamp(int64 Time);

	/** Get seconds from DateTime to UnixTimestamp. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Utils")
	static int64 GetSecondsFromUnixTimestamp(FDateTime DateTime);

	template <typename TEnum>
	static FString GetEnumValueAsString(const FString& EnumName, TEnum Value)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
		if (!EnumPtr)
		{
			return FString("Invalid");
		}
		const FString ValueStr = EnumPtr->GetNameByValue(static_cast<int64>(Value)).ToString();
		return ValueStr.Replace(*FString::Printf(TEXT("%s::"), *EnumName), TEXT(""));
	}

	template <typename EnumType>
	static EnumType GetEnumValueFromString(const FString& EnumName, const FString& String)
	{
		UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
		if (!Enum)
		{
			return EnumType(0);
		}
		return static_cast<EnumType>(Enum->GetValueByName(FName(*String)));
	}
};
