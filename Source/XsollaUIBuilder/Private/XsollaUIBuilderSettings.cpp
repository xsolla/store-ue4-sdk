// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderSettings.h"

#include "UObject/ConstructorHelpers.h"
#include "XsollaUIBuilderModule.h"
#include "XsollaUtilsLibrary.h"

UXsollaUIBuilderSettings::UXsollaUIBuilderSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UXsollaUIBuilderTheme> ThemeFinder(*FString::Printf(TEXT("/%s/UIBuilder/BP_DefaultTheme.BP_DefaultTheme_C"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaUIBuilderModule::ModuleName)));
	static ConstructorHelpers::FClassFinder<UXsollaUIBuilderWidgetsLibrary> WidgetsLibraryFinder(*FString::Printf(TEXT("/%s/UIBuilder/BP_DefaultWidgetsLibrary.BP_DefaultWidgetsLibrary_C"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaUIBuilderModule::ModuleName)));
	InterfaceTheme = ThemeFinder.Class;
	WidgetsLibrary = WidgetsLibraryFinder.Class;
}
