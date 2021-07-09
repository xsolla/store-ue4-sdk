// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUIBuilderTheme.h"
#include "XsollaUIBuilderWidgetsLibrary.h"

#include "XsollaUIBuilderSettings.generated.h"

USTRUCT(BlueprintType)
struct FEntityTypeName
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TypeAsInt;

	UPROPERTY()
	FName Name;

	FEntityTypeName()
		:TypeAsInt(INT32_MAX)
	{
	}
	
	FEntityTypeName(int32 InTypeAsInt, const FName& InName)
		: TypeAsInt(InTypeAsInt)
		, Name(InName)
	{
	}
};

UCLASS(config = Engine, defaultconfig)
class XSOLLAUIBUILDER_API UXsollaUIBuilderSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;

	/** Widget types. */
	UPROPERTY(config, EditAnywhere, Category = "Xsolla UIBuilder Settings", AdvancedDisplay)
	TArray<FEntityTypeName> WidgetTypes;

	/** Interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UIBuilder Settings")
	TSubclassOf<UXsollaUIBuilderTheme> InterfaceTheme;

	/* Widgets library. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UIBuilder Settings")
	TSubclassOf<UXsollaUIBuilderWidgetsLibrary> WidgetsLibrary;

	static UXsollaUIBuilderSettings* Get() { return CastChecked<UXsollaUIBuilderSettings>(StaticClass()->GetDefaultObject()); }

#if WITH_EDITOR
	static void LoadType(UEnum* Enum, const TArray<FEntityTypeName>& Types);
#endif
	
};
