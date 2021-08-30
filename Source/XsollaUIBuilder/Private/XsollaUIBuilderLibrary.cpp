// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderLibrary.h"
#include "XsollaUIBuilderModule.h"
#include "XsollaUIBuilderSettings.h"
#include "XsollaUtilsLibrary.h"

UXsollaUIBuilderLibrary::UXsollaUIBuilderLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaUIBuilderSettings* UXsollaUIBuilderLibrary::GetUIBuilderSettings()
{
	return FXsollaUIBuilderModule::Get().GetSettings();
}

UXsollaUIBuilderTheme* UXsollaUIBuilderLibrary::GetCurrentTheme()
{
	UXsollaUIBuilderSettings* Settings = FXsollaUIBuilderModule::Get().GetSettings();
	TSubclassOf<UXsollaUIBuilderTheme> CurrentThemeClass = Settings->InterfaceTheme;
	if (!CurrentThemeClass)
	{
		return nullptr;
	}

	UObject* CurrentTheme;
	UXsollaUtilsLibrary::GetDefaultObject(CurrentThemeClass, CurrentTheme);

	return Cast<UXsollaUIBuilderTheme>(CurrentTheme);
}

UXsollaUIBuilderTheme* UXsollaUIBuilderLibrary::GetTheme(TSubclassOf<UXsollaUIBuilderTheme> ThemeClass)
{
	if (!ThemeClass)
	{
		return nullptr;
	}

	UObject* Theme;
	UXsollaUtilsLibrary::GetDefaultObject(ThemeClass, Theme);

	return Cast<UXsollaUIBuilderTheme>(Theme);
}

UXsollaUIBuilderWidgetsLibrary* UXsollaUIBuilderLibrary::GetCurrentWidgetsLibrary()
{
	UXsollaUIBuilderSettings* Settings = FXsollaUIBuilderModule::Get().GetSettings();
	TSubclassOf<UXsollaUIBuilderWidgetsLibrary> CurrentWidgetsLibraryClass = Settings->WidgetsLibrary;
	if (!CurrentWidgetsLibraryClass)
	{
		return nullptr;
	}

	UObject* CurrentWidgetsLibrary;
	UXsollaUtilsLibrary::GetDefaultObject(CurrentWidgetsLibraryClass, CurrentWidgetsLibrary);

	return Cast<UXsollaUIBuilderWidgetsLibrary>(CurrentWidgetsLibrary);
}

UXsollaUIBuilderWidgetsLibrary* UXsollaUIBuilderLibrary::GetWidgetsLibrary(TSubclassOf<UXsollaUIBuilderWidgetsLibrary> WidgetLibraryClass)
{
	if (!WidgetLibraryClass)
	{
		return nullptr;
	}

	UObject* WidgetLibrary;
	UXsollaUtilsLibrary::GetDefaultObject(WidgetLibraryClass, WidgetLibrary);

	return Cast<UXsollaUIBuilderWidgetsLibrary>(WidgetLibrary);
}

void UXsollaUIBuilderLibrary::Clear(FThemeParameters& Parameters)
{
	Parameters.Colors.Empty();
	Parameters.Fonts.Empty();
	Parameters.Brushes.Empty();
}

bool UXsollaUIBuilderLibrary::IsEmpty(const FThemeParameters& Parameters)
{
	return Parameters.Colors.Num() == 0 && Parameters.Fonts.Num() == 0 && Parameters.Brushes.Num() == 0;
}
