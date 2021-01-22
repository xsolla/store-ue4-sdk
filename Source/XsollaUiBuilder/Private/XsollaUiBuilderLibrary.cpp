// Fill out your copyright notice in the Description page of Project Settings.


#include "XsollaUiBuilderLibrary.h"

#include "XsollaUiBuilder.h"

UXsollaUiBuilderSettings* UXsollaUiBuilderLibrary::GetUiBuilderSettings()
{
	return FXsollaUiBuilderModule::Get().GetSettings();
}

FColor UXsollaUiBuilderLibrary::GetUiBuilderColorByName(const FString& Name)
{
	const FColor* Result = GetUiBuilderCurrentTheme().Colors.Find(Name);
	if (Result)
	{
		return *Result;
	}
	return FColor::Red;
}

UClass* UXsollaUiBuilderLibrary::GetUiBuilderWidgetByName(const FString& Name)
{
	UClass** Result = GetUiBuilderCurrentTheme().Widgets.Find(Name);
	if (Result)
	{
		return *Result;
	}
	return nullptr;
}

FSlateFontInfo UXsollaUiBuilderLibrary::GetUIBuilderFontByName(const FString& Name)
{
	FSlateFontInfo* Result = GetUiBuilderCurrentTheme().Fonts.Find(Name);
	if (Result)
	{
		return *Result;
	}
	return FSlateFontInfo();
}

FSlateBrush UXsollaUiBuilderLibrary::GetUIBuilderBrushByName(const FString& Name, bool& bIsFound)
{
	FSlateBrush* Result = GetUiBuilderCurrentTheme().Brushes.Find(Name);
	if (Result)
	{
		bIsFound = true;
		return *Result;
	}
	bIsFound = false;
	return FSlateBrush();
}

FTheme UXsollaUiBuilderLibrary::GetUiBuilderCurrentTheme()
{
	const UXsollaUiBuilderSettings* Settings = GetUiBuilderSettings();

	FTheme Theme = FTheme();
	if (Settings->bUseThemeObject)
	{
		const auto ThemeInstance = UThemeObject::GetThemeInstance();
		if (ThemeInstance == nullptr)
		{
			UE_LOG(LogActor, Error, TEXT("We found nullptr ThemeObject"));
		}
		else
		{
			if (Theme.Colors.Num() != 0)
			{
				Theme.Colors = ThemeInstance->Colors;
				Theme.Widgets = ThemeInstance->Widgets;
				// Theme.Fonts = ThemeInstance->Fonts;
			}
		}
		// const auto Colors = FindField<TMap<FString, FColor>>(Settings->CurrentTheme, "Colors");
		// if (Colors)
		// {
		// 	Theme.Colors = *Colors;
		// }
		//
		// const auto Widgets = FindField<TMap<FString, UClass*>>(Settings->CurrentTheme, "Widgets");
		// if (Widgets)
		// {
		// 	Theme.Widgets = *Widgets;
		// }
	}
	else
	{
		const int CurrentThemeIndex = Settings->CurrentThemeIndex;
		if (CurrentThemeIndex >= Settings->Themes.Num())
		{
			Theme = FTheme();
		}
		else
		{
			Theme = Settings->Themes[CurrentThemeIndex];
		}
	}

	return Theme;
}
