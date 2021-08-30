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

	// default widget types
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType1), FName("Button")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType2), FName("ButtonCounter")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType3), FName("IconTextButton")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType4), FName("ButtonToggle")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType5), FName("SmallIconButton")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType6), FName("ButtonMenuMain")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType7), FName("ButtonIconStates")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType8), FName("Link")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType9), FName("Checkbox")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType10), FName("Image")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType11), FName("Icon")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType12), FName("Input")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType13), FName("InputSearch")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType14), FName("EditableText")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType15), FName("Text")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType16), FName("RichText")));
	WidgetTypes.Add(FEntityTypeName(static_cast<int32>(WidgetType17), FName("InputExecute")));

	// default color types
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType1), FName("Transparent")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType2), FName("Main")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType3), FName("Accent")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType4), FName("Inactive")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType5), FName("Inactive2")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType6), FName("AccentDull")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType7), FName("Darkening")));
	ColorTypes.Add(FEntityTypeName(static_cast<int32>(ColorType8), FName("Inactive3")));

	// default font types
	FontTypes.Add(FEntityTypeName(static_cast<int32>(FontType1), FName("MainBold")));
	FontTypes.Add(FEntityTypeName(static_cast<int32>(FontType2), FName("MainRegular")));

	// default brush types
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType1), FName("MainButtonInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType2), FName("MainButtonHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType3), FName("MainButtonPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType4), FName("MainButtonDisabled")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType5), FName("NormalButtonInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType6), FName("NormalButtonHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType7), FName("NormalButtonPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType8), FName("NormalButtonDisabled")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType9), FName("IconButtonInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType10), FName("IconButtonHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType11), FName("IconButtonPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType12), FName("IconButtonDisabled")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType13), FName("CheckBoxUncheckedInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType14), FName("CheckBoxUncheckedHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType15), FName("CheckBoxUncheckedPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType16), FName("CheckBoxCheckedInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType17), FName("CheckBoxCheckedHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType18), FName("CheckBoxCheckedPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType19), FName("PopupMenuButtonClosedInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType20), FName("PopupMenuButtonClosedHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType21), FName("PopupMenuButtonClosedPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType22), FName("PopupMenuButtonClosedDisabled")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType23), FName("PopupMenuButtonOpenedInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType24), FName("PopupMenuButtonOpenedHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType25), FName("PopupMenuButtonOpenedPressed")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType26), FName("PopupMenuButtonOpenedDisabled")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType27), FName("InputInitial")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType28), FName("InputHover")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType29), FName("InputActive")));
	BrushTypes.Add(FEntityTypeName(static_cast<int32>(BrushType30), FName("InputDisabled")));
}

void UXsollaUIBuilderSettings::PostInitProperties()
{
	Super::PostInitProperties();
#if WITH_EDITOR
	LoadType(StaticEnum<EWidgetType>(), WidgetTypes);
	LoadType(StaticEnum<EColorType>(), ColorTypes);
	LoadType(StaticEnum<EFontType>(), FontTypes);
	LoadType(StaticEnum<EBrushThemeType>(), BrushTypes);
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

void UXsollaUIBuilderSettings::UpdateTheme(TSubclassOf<UXsollaUIBuilderTheme> NewTheme)
{
	InterfaceTheme = NewTheme;
	ThemeChanged.Broadcast(InterfaceTheme);
}
