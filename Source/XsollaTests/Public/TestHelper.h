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

	/** Find UI Element by name over all widgets */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass, WorldContext = "WorldContextObject"))
    static UWidget* FindUIElement(UObject* WorldContextObject, const FName Name, const TSubclassOf<UWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass))
    static UWidget* FindUIElementInWidget(const UUserWidget* Parent, FName Name, TSubclassOf<UWidget> WidgetClass);

	/** Set input field value by text, if success - return true */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
    static bool SetInputFieldText(UObject* WorldContextObject, const FName InputFieldName, const FText Text, const bool bFindAsWidget);

	/** Find button by name and click */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
	static bool ClickButtonByName(UObject* WorldContextObject, FName ButtonName, bool bFindAsWidget);

	/** Click at button */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
	static void ClickButton(UObject* WorldContextObject, UButton* Button);

	/** Find button widget */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
	static UWidget* FindButtonWidget(UObject* WorldContextObject, FName ButtonName, bool bFindAsWidget);

	static bool ClickButton_Internal(UWidget* Widget);

	/** Find Input widget by Name */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (WorldContext = "WorldContextObject"))
    static UWidget* FindInputWidget(UObject* WorldContextObject, FName InputFieldName, bool bFindAsWidget);

	/** Internal method which setup text for widget, if success - return true */
	static bool SetInputFieldText_Internal(UWidget* Widget, FText Text);

	/** Find Widget by class, if few widgets exists - return first entire */
	UFUNCTION(BlueprintCallable, Category = "TestHelper|UI", meta = (DeterminesOutputType = WidgetClass, WorldContext = "WorldContextObject"))
    static UUserWidget* FindWidgetByClass(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass);

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

	/** Write some text to current focused element */
	UFUNCTION(BlueprintCallable, Category = "TestHelper", meta = (WorldContext = "WorldContextObject"))
	static bool WriteText2(UObject* WorldContextObject, const FString& Text);
};
