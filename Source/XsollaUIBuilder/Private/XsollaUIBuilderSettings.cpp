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

	WidgetTypes.Add(FWidgetTypeName(WidgetType1, FName("Button")));
	WidgetTypes.Add(FWidgetTypeName(WidgetType2, FName("ButtonCounter")));
	WidgetTypes.Add(FWidgetTypeName(WidgetType3, FName("IconTextButton")));
}

void UXsollaUIBuilderSettings::PostInitProperties()
{
	Super::PostInitProperties();
#if WITH_EDITOR
	LoadWidgetType();
#endif
}

#if WITH_EDITOR
void UXsollaUIBuilderSettings::LoadWidgetType()
{
	// read "WidgetTYpe" defines and set meta data for the enum
	// find the enum
	UEnum* Enum = StaticEnum<EWidgetType>();
	// we need this Enum
	check(Enum);

	const FString KeyName = TEXT("DisplayName");
	const FString HiddenMeta = TEXT("Hidden");
	const FString UnusedDisplayName = TEXT("Unused");

	// remainders, set to be unused
	for (int32 EnumIndex = 1; EnumIndex < Enum->NumEnums(); ++EnumIndex)
	{
		if (!Enum->HasMetaData(*HiddenMeta, EnumIndex))
		{
			Enum->SetMetaData(*HiddenMeta, TEXT(""), EnumIndex);
			Enum->SetMetaData(*KeyName, *UnusedDisplayName, EnumIndex);
		}
	}

	for (auto Iter = WidgetTypes.CreateConstIterator(); Iter; ++Iter)
	{
		Enum->SetMetaData(*KeyName, *Iter->Name.ToString(), Iter->Type);
		// also need to remove "Hidden"
		Enum->RemoveMetaData(*HiddenMeta, Iter->Type);
	}
}
#endif
