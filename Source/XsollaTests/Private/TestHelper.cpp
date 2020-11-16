// Fill out your copyright notice in the Description page of Project Settings.

#include "TestHelper.h"

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
#include "AutomationDriver/Public/AutomationDriverTypeDefs.h"
#include "AutomationDriver/Public/IAutomationDriver.h"
#include "AutomationDriver/Public/IAutomationDriverModule.h"
#include "AutomationDriver/Public/IDriverElement.h"
#include "AutomationDriver/Public/LocateBy.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Internationalization/Regex.h"

UWidget* UTestHelper::FindUIElement(UObject* WorldContextObject, const FName Name,
	const TSubclassOf<UWidget> WidgetClass)
{
	TArray<UUserWidget*> Widgets;
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, Widgets, UUserWidget::StaticClass(), true);
	for (auto Widget : Widgets)
	{
		if (auto const Element = FindUIElementInWidget(Widget, Name, WidgetClass))
		{
			return Element;
		}
	}

	return nullptr;
}

UWidget* UTestHelper::FindUIElementInWidget(const UUserWidget* Parent, const FName Name,
	const TSubclassOf<UWidget> WidgetClass)
{
	TArray<UUserWidget*> Widgets;
	UWidget* OutWidget = nullptr;

	Parent->WidgetTree->ForEachWidget(
		[&](UWidget* Widget)
		{
			const FRegexPattern ElementPattern(Name.ToString());
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
		if (auto const Element = FindUIElementInWidget(Widget, Name, WidgetClass))
		{
			if (Element->GetClass()->IsChildOf(WidgetClass.Get()))
			{
				return Element;
			}
		}
	}

	return nullptr;
}

bool UTestHelper::SetInputFieldText(UObject* WorldContextObject, const FName InputFieldName, const FText Text,
	const bool bFindAsWidget)
{
	UWidget* Widget = FindInputWidget(WorldContextObject, InputFieldName, bFindAsWidget);

	if (Widget == nullptr)
	{
		return false;
	}

	return SetInputFieldText_Internal(Widget, Text);
}

bool UTestHelper::ClickButtonByName(UObject* WorldContextObject, const FName ButtonName, const bool bFindAsWidget)
{
	UWidget* Widget = FindButtonWidget(WorldContextObject, ButtonName, bFindAsWidget);

	if (Widget == nullptr)
	{
		return false;
	}

	return ClickButton_Internal(Widget);
}

void UTestHelper::ClickButton(UObject* WorldContextObject, UButton* Button)
{
	ClickButton_Internal(Button);
}

UWidget* UTestHelper::FindButtonWidget(UObject* WorldContextObject, const FName ButtonName, const bool bFindAsWidget)
{
	UWidget* Widget = FindUIElement(WorldContextObject, ButtonName, UWidget::StaticClass());
	if (Widget == nullptr)
	{
		return nullptr;
	}

	if (bFindAsWidget)
	{
		if (auto const UserWidget = Cast<UUserWidget>(Widget))
		{
			Widget = FindUIElementInWidget(UserWidget, TEXT(".*"), UButton::StaticClass());
		}
	}

	return Widget;
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

UWidget* UTestHelper::FindInputWidget(UObject* WorldContextObject, const FName InputFieldName, const bool bFindAsWidget)
{
	UWidget* Widget = FindUIElement(WorldContextObject, InputFieldName, UWidget::StaticClass());
	if (Widget == nullptr)
	{
		return nullptr;
	}

	if (bFindAsWidget)
	{
		if (auto const UserWidget = Cast<UUserWidget>(Widget))
		{
			Widget = FindUIElementInWidget(UserWidget, TEXT(".*"), UEditableTextBox::StaticClass());
			if (Widget == nullptr)
			{
				Widget = FindUIElementInWidget(UserWidget, TEXT(".*"), UEditableText::StaticClass());
			}
		}
	}

	return Widget;
}

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

UUserWidget* UTestHelper::FindWidgetByClass(UObject* WorldContextObject, const TSubclassOf<UUserWidget> WidgetClass)
{
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(WorldContextObject, Widgets, WidgetClass, false);

	return Widgets.Num() != 0 ? Widgets[0] : nullptr;
}

void UTestHelper::RestoreDefaultSettings()
{
	UXsollaLoginLibrary::GetLoginSettings()->SetupDefaultDemoSettings();
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
}

bool UTestHelper::FinishTest(
	AFunctionalTest* Test, const bool bIsSuccess,
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

bool UTestHelper::WriteText2(UObject* WorldContextObject, const FString& Text)
{
	IAutomationDriverModule::Get().Enable();

	FAutomationDriverPtr Driver = IAutomationDriverModule::Get().CreateDriver();
	FDriverElementPtr Element = Driver->FindElement(By::Cursor());
	Element->Type(Text);

	Driver.Reset();
	IAutomationDriverModule::Get().Disable();

	return true;
}
