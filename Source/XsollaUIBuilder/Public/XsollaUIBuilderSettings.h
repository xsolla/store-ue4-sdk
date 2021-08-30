// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaUIBuilderTheme.h"
#include "XsollaUIBuilderTypes.h"
#include "XsollaUIBuilderWidgetsLibrary.h"

#include "XsollaUIBuilderSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThemeChanged, TSubclassOf<UXsollaUIBuilderTheme>, NewTheme);

UCLASS(config = Engine, defaultconfig)
class XSOLLAUIBUILDER_API UXsollaUIBuilderSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;

#if WITH_EDITOR
	static void LoadType(UEnum* Enum, const TArray<FEntityTypeName>& Types);
#endif

	UFUNCTION(BlueprintCallable, Category = "Xsolla UIBuilder Settings")
	void UpdateTheme(TSubclassOf<UXsollaUIBuilderTheme> NewTheme);

	UPROPERTY(BlueprintAssignable, Category = "Xsolla UIBuilder Settings")
	FOnThemeChanged ThemeChanged;

	/** Widget types. */
	UPROPERTY(config, EditAnywhere, Category = "Xsolla UIBuilder Settings", AdvancedDisplay)
	TArray<FEntityTypeName> WidgetTypes;

	/** Color types. */
	UPROPERTY(config, EditAnywhere, Category = "Xsolla UIBuilder Settings", AdvancedDisplay)
	TArray<FEntityTypeName> ColorTypes;

	/** Font types. */
	UPROPERTY(config, EditAnywhere, Category = "Xsolla UIBuilder Settings", AdvancedDisplay)
	TArray<FEntityTypeName> FontTypes;

	/** Brush types. */
	UPROPERTY(config, EditAnywhere, Category = "Xsolla UIBuilder Settings", AdvancedDisplay)
	TArray<FEntityTypeName> BrushTypes;

	/** Interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla UIBuilder Settings", meta = (AllowPrivateAccess = true))
	TSubclassOf<UXsollaUIBuilderTheme> InterfaceTheme;

	/* Widgets library. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla UIBuilder Settings", meta = (AllowPrivateAccess = true))
	TSubclassOf<UXsollaUIBuilderWidgetsLibrary> WidgetsLibrary;
};
