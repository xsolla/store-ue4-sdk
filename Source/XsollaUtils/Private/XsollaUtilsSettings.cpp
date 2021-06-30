// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsSettings.h"

#include "UObject/ConstructorHelpers.h"
#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsModule.h"

UXsollaUtilsSettings::UXsollaUtilsSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UXsollaUtilsTheme> ThemeFinder(*FString::Printf(TEXT("/%s/Utils/BP_DefaultTheme.BP_DefaultTheme_C"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaUtilsModule::ModuleName)));
	static ConstructorHelpers::FClassFinder<UXsollaUtilsWidgetsLibrary> WidgetsLibraryFinder(*FString::Printf(TEXT("/%s/Utils/BP_DefaultWidgetsLibrary.BP_DefaultWidgetsLibrary_C"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaUtilsModule::ModuleName)));
	InterfaceTheme = ThemeFinder.Class;
	WidgetsLibrary = WidgetsLibraryFinder.Class;
}
