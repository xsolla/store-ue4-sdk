// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "FunctionalTest.h"
#include "Components/Button.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/Widget.h"
#include "UObject/NoExportTypes.h"
#include "TestHelper.generated.h"

/**
* Helper class
*/
UCLASS()
class XSOLLATESTS_API UTestHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//~=============================================================================
	// Basic functions.

	/** Find UI Element by name over all widgets */
    static UWidget* FindUIElementInternal(UObject* WorldContextObject, const FString& RegExp, const TSubclassOf<UWidget> WidgetClass);

	/** Find UI Element by regular expression over all widgets */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass, WorldContext = "WorldContextObject"))
    static UWidget* FindUIElementByRegExp(UObject* WorldContextObject, const FString& RegExp, const TSubclassOf<UWidget> WidgetClass);

	/** Find UI Element by name over all widgets */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass, WorldContext = "WorldContextObject"))
    static UWidget* FindUIElementByName(UObject* WorldContextObject, const FName Name, const TSubclassOf<UWidget> WidgetClass);

    static UWidget* FindUIElementInWidgetInternal(const UUserWidget* Parent, const FString& RegExp, TSubclassOf<UWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass))
    static UWidget* FindUIElementInWidgetByRegExp(const UUserWidget* Parent, const FString& RegExp, TSubclassOf<UWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass))
    static UWidget* FindUIElementInWidgetByName(const UUserWidget* Parent, const FName Name, TSubclassOf<UWidget> WidgetClass);

	/** Find Widget by class, if few widgets exists - return first entire */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass, WorldContext = "WorldContextObject"))
    static UUserWidget* FindWidgetByClass(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass);

	/** Find button widget */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
    static UWidget* FindButtonWidget(UObject* WorldContextObject, FName ButtonName, bool bFindAsWidget);

	/** Find Input widget by Name */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
    static UWidget* FindInputWidget(UObject* WorldContextObject, FName InputFieldName, bool bFindAsWidget);

	//~=============================================================================
	// Helpers.

	/** Setup default settings for new test as default demo */
	UFUNCTION(BlueprintCallable, Category = "TestHelper")
    static void RestoreDefaultSettings();

	/** Setup default playground settings - for example, clear login data */
	UFUNCTION(BlueprintCallable, Category = "TestHelper", meta = (WorldContext = "WorldContextObject"))
    static void SetupTestPlayground(UObject* WorldContextObject);

	/** Create start Login widget */
	UFUNCTION(BlueprintCallable, Category = "TestHelper", meta = (WorldContext = "WorldContextObject"))
    static void CreateLoginWidget(UObject* WorldContextObject);

	/** Finish test wrapper */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|Tests", meta = (DefaultToSelf = "Test"))
    static bool FinishTest(AFunctionalTest* Test, bool bIsSuccess, const FString& SuccessMessage, const FString& FailureMessage);

	//~=============================================================================
	// Helpers UI.

	/** Internal method which setup text for widget, if success - return true */
	static bool SetInputFieldText_Internal(UWidget* Widget, FText Text);

	/** Set input field value by text, if success - return true */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
    static bool SetInputFieldText(UObject* WorldContextObject, const FName InputFieldName, const FText Text, const bool bFindAsWidget);

	static bool ClickButton_Internal(UWidget* Widget);
	
	/** Click at button */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
	static void ClickButton(UObject* WorldContextObject, UButton* Button);

	/** Find button by name and click */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
    static bool ClickButtonByName(UObject* WorldContextObject, FName ButtonName, bool bFindAsWidget);

	UFUNCTION(BlueprintCallable, Category = "TestHelper|Tests")
	static bool HelperCallTab(bool bForward);

	/** Write some text to current focused element */
	UFUNCTION(BlueprintCallable, Category = "TestHelper")
	static bool HelperWriteText(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "TestHelper|Tests")
	static bool HelperClickButton();
};
