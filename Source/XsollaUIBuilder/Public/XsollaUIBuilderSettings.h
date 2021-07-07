// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUIBuilderTheme.h"
#include "XsollaUIBuilderWidgetsLibrary.h"
#include "XsollaUIBuilderTypes.h"

#include "XsollaUIBuilderSettings.generated.h"

/**
 * Structure that represents the name of widget types.
 */
USTRUCT(BlueprintType)
struct FWidgetTypeName
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TEnumAsByte<enum EWidgetType> Type;

	UPROPERTY()
	FName Name;

	FWidgetTypeName()
		: Type(WidgetType_Max)
	{
	}
	FWidgetTypeName(EWidgetType InType, const FName& InName)
		: Type(InType)
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
	TArray<FWidgetTypeName> WidgetTypes;
	
	/** Interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UIBuilder Settings")
	TSubclassOf<UXsollaUIBuilderTheme> InterfaceTheme;

	/* Widgets library. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla UIBuilder Settings")
	TSubclassOf<UXsollaUIBuilderWidgetsLibrary> WidgetsLibrary;

	static UXsollaUIBuilderSettings* Get() { return CastChecked<UXsollaUIBuilderSettings>(StaticClass()->GetDefaultObject()); }

#if WITH_EDITOR
	void LoadWidgetType();
#endif
	
};
