// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "EngineMinimal.h"

#include "XsollaUIBuilderLibrary.generated.h"

class UXsollaUIBuilderSettings;
class UXsollaUIBuilderTheme;
class UXsollaUIBuilderWidgetsLibrary;

UCLASS()
class XSOLLAUIBUILDER_API UXsollaUIBuilderLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	/** Gives direct access to the UIBuilder settings. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|UIBuilder")
	static UXsollaUIBuilderSettings* GetUIBuilderSettings();
	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Xsolla|UIBuilder")
	static UXsollaUIBuilderTheme* GetCurrentTheme();
	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Xsolla|UIBuilder")
	static UXsollaUIBuilderTheme* GetTheme(TSubclassOf<UXsollaUIBuilderTheme> ThemeClass);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Xsolla|UIBuilder")
	static UXsollaUIBuilderWidgetsLibrary* GetCurrentWidgetsLibrary();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Xsolla|UIBuilder")
	static UXsollaUIBuilderWidgetsLibrary* GetWidgetsLibrary(TSubclassOf<UXsollaUIBuilderWidgetsLibrary> WidgetLibraryClass);
};
