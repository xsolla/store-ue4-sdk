// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderSettings.h"

#include "UObject/ConstructorHelpers.h"
#include "XsollaUIBuilderModule.h"
#include "XsollaUIBuilderTypes.h"
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

	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType1), FName("Button")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType2), FName("ButtonCounter")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType3), FName("IconTextButton")));
}

void UXsollaUIBuilderSettings::PostInitProperties()
{
	Super::PostInitProperties();
#if WITH_EDITOR
	LoadType(StaticEnum<EWidgetType>(), WidgetTypes);
#endif
}

#if WITH_EDITOR
void UXsollaUIBuilderSettings::LoadType(UEnum* Enum, const TArray<FEntityTypeName>& Types)
{
	check(Enum);

	const FString KeyName = TEXT("DisplayName");
	const FString HiddenMeta = TEXT("Hidden");
	const FString UnusedDisplayName = TEXT("Unused");

	for (int32 EnumIndex = 1; EnumIndex < Enum->NumEnums(); ++EnumIndex)
	{
		if (!Enum->HasMetaData(*HiddenMeta, EnumIndex))
		{
			Enum->SetMetaData(*HiddenMeta, TEXT(""), EnumIndex);
			Enum->SetMetaData(*KeyName, *UnusedDisplayName, EnumIndex);
		}
	}

	for (auto Iter = Types.CreateConstIterator(); Iter; ++Iter)
	{
		if (Iter->TypeAsInt > 0 && Iter->TypeAsInt < INT32_MAX)
		{
			Enum->SetMetaData(*KeyName, *Iter->Name.ToString(), Iter->TypeAsInt);
			Enum->RemoveMetaData(*HiddenMeta, Iter->TypeAsInt);
		}
	}
}
#endif
