// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsSettings.h"
#include "UObject/ConstructorHelpers.h"

UXsollaUtilsSettings::UXsollaUtilsSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UXsollaUtilsTheme> ThemeFinder(TEXT("/Xsolla/Utils/BP_DefaultTheme.BP_DefaultTheme_C"));
	static ConstructorHelpers::FClassFinder<UXsollaUtilsWidgetsLibrary> WidgetsLibraryFinder(TEXT("/Xsolla/Utils/BP_DefaultWidgetsLibrary.BP_DefaultWidgetsLibrary_C"));
	InterfaceTheme = ThemeFinder.Class;
	WidgetsLibrary = WidgetsLibraryFinder.Class;
}
