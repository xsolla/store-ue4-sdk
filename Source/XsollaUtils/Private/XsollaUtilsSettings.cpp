// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsSettings.h"

#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsModule.h"
#include "UObject/ConstructorHelpers.h"

UXsollaUtilsSettings::UXsollaUtilsSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	const FString PluginName = UXsollaUtilsLibrary::GetPluginName(FXsollaUtilsModule::ModuleName);
	const FString ThemePath = FString::Printf(TEXT("/%s/Utils/BP_DefaultTheme.BP_DefaultTheme_C"), *PluginName);
	const FString WidgetsLibraryPath = FString::Printf(TEXT("/%s/Utils/BP_DefaultWidgetsLibrary.BP_DefaultWidgetsLibrary_C"), *PluginName);
	static ConstructorHelpers::FClassFinder<UXsollaUtilsTheme> ThemeFinder(*ThemePath);
	static ConstructorHelpers::FClassFinder<UXsollaUtilsWidgetsLibrary> WidgetsLibraryFinder(*WidgetsLibraryPath);
	InterfaceTheme = ThemeFinder.Class;
	WidgetsLibrary = WidgetsLibraryFinder.Class;
}
