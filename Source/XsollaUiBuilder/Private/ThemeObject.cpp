// Fill out your copyright notice in the Description page of Project Settings.


#include "ThemeObject.h"

#include "Templates/SubclassOf.h"
#include "XsollaUiBuilderLibrary.h"

UThemeObject* UThemeObject::ThemeInstance;
TSubclassOf<UThemeObject> UThemeObject::ThemeInstanceClass;

void UThemeObject::NewThemeInstance()
{
	UXsollaUiBuilderSettings* Settings = UXsollaUiBuilderLibrary::GetUiBuilderSettings();
	const TSubclassOf<UThemeObject> CurrentSelectedThemeClass = Settings->CurrentTheme;
	if (CurrentSelectedThemeClass == nullptr)
	{
		return;
	}
	ThemeInstance = NewObject<UThemeObject>(GetTransientPackage(), CurrentSelectedThemeClass, "Name", RF_Standalone);
	ThemeInstance->AddToRoot();
	ThemeInstanceClass = CurrentSelectedThemeClass;
}

UThemeObject* UThemeObject::GetThemeInstance()
{
	UXsollaUiBuilderSettings* Settings = UXsollaUiBuilderLibrary::GetUiBuilderSettings();
	const TSubclassOf<UThemeObject> CurrentSelectedThemeClass = Settings->CurrentTheme;
	if (ThemeInstance == nullptr)
	{
		if (CurrentSelectedThemeClass != nullptr)
		{
			NewThemeInstance();
		}
	}
	else
	{
		if (CurrentSelectedThemeClass != ThemeInstanceClass)
		{	
			NewThemeInstance();
		}

		if (ThemeInstance->HasAnyFlags(RF_BeginDestroyed | RF_ClassDefaultObject))
		{
			NewThemeInstance();
		}
	}

	return ThemeInstance;
}
