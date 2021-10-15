// Fill out your copyright notice in the Description page of Project Settings.

#include "TestHelper.h"

#include "Async/Async.h"
#include "Engine/Engine.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "XsollaLogin/Public/XsollaLoginSubsystem.h"
#include "XsollaDemoGameModeBase.h"
#include "XsollaStore/Public/XsollaStoreLibrary.h"
#include "XsollaStore/Public/XsollaStoreSettings.h"
#include "XsollaLogin/Public/XsollaLoginLibrary.h"
#include "XsollaLogin/Public/XsollaLoginSettings.h"
#include "XsollaInventory/Public/XsollaInventoryLibrary.h"
#include "XsollaInventory/Public/XsollaInventorySettings.h"
#include "AutomationDriver/Public/AutomationDriverTypeDefs.h"
#include "AutomationDriver/Public/IAutomationDriver.h"
#include "AutomationDriver/Public/IAutomationDriverModule.h"
#include "AutomationDriver/Public/IDriverElement.h"
#include "AutomationDriver/Public/LocateBy.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Internationalization/Regex.h"

//~=============================================================================
// Basic functions.

UWidget* UTestHelper::FindUIElementInternal(UObject* WorldContextObject, const FString& RegExp, const TSubclassOf<UWidget> WidgetClass)
{
	TArray<UUserWidget*> Widgets;
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, Widgets, UUserWidget::StaticClass(), true);
	for (auto Widget : Widgets)
	{
		if (auto const Element = FindUIElementInWidgetByRegExp(Widget, RegExp, WidgetClass))
		{
			return Element;
		}
	}

	return nullptr;
}

UWidget* UTestHelper::FindUIElementByRegExp(UObject* WorldContextObject, const FString& RegExp,
	const TSubclassOf<UWidget> WidgetClass)
{
	return FindUIElementInternal(WorldContextObject, RegExp, WidgetClass);
}

UWidget* UTestHelper::FindUIElementByName(UObject* WorldContextObject, const FName Name, const TSubclassOf<UWidget> WidgetClass)
{
	const FString RegExp = FString::Printf(TEXT("^%s$"), *Name.ToString());
	return FindUIElementInternal(WorldContextObject, RegExp, WidgetClass);
}

UWidget* UTestHelper::FindUIElementInWidgetInternal(const UUserWidget* Parent, const FString& RegExp,
	const TSubclassOf<UWidget> WidgetClass)
{
	TArray<UUserWidget*> Widgets;
	UWidget* OutWidget = nullptr;

	Parent->WidgetTree->ForEachWidget(
		[&](UWidget* Widget)
		{
			const FRegexPattern ElementPattern(RegExp);
			FRegexMatcher Matcher(ElementPattern, Widget->GetName());

			if (Matcher.FindNext() && Widget->GetClass()->IsChildOf(WidgetClass.Get()))
			{
				OutWidget = Widget;
				return;
			}

			if (const auto UserWidget = Cast<UUserWidget>(Widget))
			{
				Widgets.Add(UserWidget);
			}
		});

	if (OutWidget != nullptr)
	{
		return OutWidget;
	}

	for (auto Widget : Widgets)
	{
		if (auto const Element = FindUIElementInWidgetInternal(Widget, RegExp, WidgetClass))
		{
			if (Element->GetClass()->IsChildOf(WidgetClass.Get()))
			{
				return Element;
			}
		}
	}

	return nullptr;
}

UWidget* UTestHelper::FindUIElementInWidgetByRegExp(const UUserWidget* Parent, const FString& RegExp, TSubclassOf<UWidget> WidgetClass)
{
	return FindUIElementInWidgetInternal(Parent, RegExp, WidgetClass);
}

UWidget* UTestHelper::FindUIElementInWidgetByName(const UUserWidget* Parent, const FName Name, TSubclassOf<UWidget> WidgetClass)
{
	const FString RegExp = FString::Printf(TEXT("^%s$"), *Name.ToString());
	return FindUIElementInWidgetInternal(Parent, RegExp, WidgetClass);
}

UUserWidget* UTestHelper::FindWidgetByClass(UObject* WorldContextObject, const TSubclassOf<UUserWidget> WidgetClass)
{
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(WorldContextObject, Widgets, WidgetClass, false);

	return Widgets.Num() != 0 ? Widgets[0] : nullptr;
}

UWidget* UTestHelper::FindButtonWidget(UObject* WorldContextObject, const FName ButtonName, const bool bFindAsWidget)
{
	UWidget* Widget = FindUIElementByName(WorldContextObject, ButtonName, UWidget::StaticClass());
	if (Widget == nullptr)
	{
		return nullptr;
	}

	if (bFindAsWidget)
	{
		if (auto const UserWidget = Cast<UUserWidget>(Widget))
		{
			Widget = FindUIElementInWidgetByRegExp(UserWidget, TEXT(".*"), UButton::StaticClass());
		}
	}

	return Widget;
}

UWidget* UTestHelper::FindInputWidget(UObject* WorldContextObject, const FName InputFieldName, const bool bFindAsWidget)
{
	UWidget* Widget = FindUIElementByName(WorldContextObject, InputFieldName, UWidget::StaticClass());
	if (Widget == nullptr)
	{
		return nullptr;
	}

	if (bFindAsWidget)
	{
		if (auto const UserWidget = Cast<UUserWidget>(Widget))
		{
			Widget = FindUIElementInWidgetByRegExp(UserWidget, TEXT(".*"), UEditableTextBox::StaticClass());
			if (Widget == nullptr)
			{
				Widget = FindUIElementInWidgetByRegExp(UserWidget, TEXT(".*"), UEditableText::StaticClass());
			}
		}
	}

	return Widget;
}

//~=============================================================================
// Helpers.

void UTestHelper::RestoreDefaultSettings()
{
	UXsollaLoginLibrary::GetLoginSettings()->SetupDefaultDemoSettings();
	UXsollaInventoryLibrary::GetInventorySettings()->SetupDefaultDemoSettings();
	UXsollaStoreLibrary::GetStoreSettings()->SetupDefaultDemoSettings();
}

void UTestHelper::SetupTestPlayground(UObject* WorldContextObject)
{
	UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UXsollaLoginSubsystem>()->DropLoginData();
}

void UTestHelper::CreateLoginWidget(UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	AXsollaDemoGameModeBase* DemoGameMode = Cast<AXsollaDemoGameModeBase>(GameMode);
	UUserWidget* Widget = UWidgetBlueprintLibrary::Create(
		WorldContextObject, DemoGameMode->GetLoginDemo(),
		UGameplayStatics::GetPlayerControllerFromID(WorldContextObject, 0));
	Widget->AddToPlayerScreen();
	APlayerController* Controller = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	Controller->SetInputMode(FInputModeUIOnly());
	Controller->bShowMouseCursor = true;
}

bool UTestHelper::FinishTest(AFunctionalTest* Test, const bool bIsSuccess,
	const FString& SuccessMessage, const FString& FailureMessage)
{
	if (!IsValid(Test))
	{
		return false;
	}
	const EFunctionalTestResult TestResult = bIsSuccess ? EFunctionalTestResult::Succeeded : EFunctionalTestResult::Failed;
	const FString Message = bIsSuccess ? SuccessMessage : FailureMessage;
	Test->FinishTest(TestResult, Message);

	return true;
}

//~=============================================================================
// Helpers UI.

bool UTestHelper::SetInputFieldText_Internal(UWidget* Widget, const FText Text)
{
	if (auto EditableTextBox = Cast<UEditableTextBox>(Widget))
	{
		EditableTextBox->SetText(Text);
		return true;
	}

	if (auto EditableText = Cast<UEditableText>(Widget))
	{
		EditableText->SetText(Text);
		return true;
	}

	return false;
}

bool UTestHelper::SetInputFieldText(UObject* WorldContextObject,
	const FName InputFieldName, const FText Text, const bool bFindAsWidget)
{
	UWidget* Widget = FindInputWidget(WorldContextObject, InputFieldName, bFindAsWidget);

	if (Widget == nullptr)
	{
		return false;
	}

	return SetInputFieldText_Internal(Widget, Text);
}

bool UTestHelper::ClickButton_Internal(UWidget* Widget)
{
	if (auto const Button = Cast<UButton>(Widget))
	{
		Button->OnClicked.Broadcast();
		return true;
	}

	return false;
}

void UTestHelper::ClickButton(UButton* Button)
{
	ClickButton_Internal(Button);
}

bool UTestHelper::ClickButtonByName(UObject* WorldContextObject,
	const FName ButtonName, const bool bFindAsWidget)
{
	UWidget* Widget = FindButtonWidget(WorldContextObject, ButtonName, bFindAsWidget);

	if (Widget == nullptr)
	{
		return false;
	}

	return ClickButton_Internal(Widget);
}

bool UTestHelper::HelperCallTab(bool bForward)
{
	if (IAutomationDriverModule::Get().IsEnabled())
	{
		IAutomationDriverModule::Get().Disable();
	}
	IAutomationDriverModule::Get().Enable();

	AsyncTask(ENamedThreads::AnyThread, [bForward]
	{
		FAutomationDriverPtr Driver = IAutomationDriverModule::Get().CreateDriver();
		FDriverElementPtr Element = Driver->FindElement(By::Cursor());
		if (bForward)
		{
			Element->Type(EKeys::Tab);
		}
		else
		{
			Element->TypeChord(EKeys::LeftShift, EKeys::Tab);
		}

		Driver.Reset();
		AsyncTask(ENamedThreads::GameThread, []
		{
			IAutomationDriverModule::Get().Disable();
		});
	});

	return true;
}

bool UTestHelper::HelperClickButton()
{
	if (IAutomationDriverModule::Get().IsEnabled())
	{
		IAutomationDriverModule::Get().Disable();
	}
	IAutomationDriverModule::Get().Enable();

	AsyncTask(ENamedThreads::AnyThread, []
	{
		FAutomationDriverPtr Driver = IAutomationDriverModule::Get().CreateDriver();
		FDriverElementPtr Element = Driver->FindElement(By::Cursor());
		Element->Click();

		Driver.Reset();
		AsyncTask(ENamedThreads::GameThread, []
		{
			IAutomationDriverModule::Get().Disable();
		});
	});

	return true;
}
