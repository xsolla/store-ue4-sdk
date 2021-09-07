// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderSettingsDetails.h"

#include "DetailLayoutBuilder.h"
#include "IDocumentation.h"
#include "PropertyEditing.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SToolTip.h"
#include "XsollaEntityTypeList.h"
#include "XsollaUIBuilderEditorTypes.h"
#include "XsollaUIBuilderLibrary.h"
#include "XsollaUIBuilderSettings.h"
#include "XsollaUIBuilderTypes.h"

#define LOCTEXT_NAMESPACE "XsollaUIBuilderSettingsDetails"

TSharedRef<IDetailCustomization> FXsollaUIBuilderSettingsDetails::MakeInstance()
{
	return MakeShareable(new FXsollaUIBuilderSettingsDetails);
}

void FXsollaUIBuilderSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	XsollaUIBuilderSettings = UXsollaUIBuilderLibrary::GetUIBuilderSettings();
	check(XsollaUIBuilderSettings);

	// widget types
	FTypeParametersStruct WidgetTypeParameters;
	WidgetTypeParameters.CategoryName = TEXT("Widget types");
	WidgetTypeParameters.PropertyPath = GET_MEMBER_NAME_CHECKED(UXsollaUIBuilderSettings, WidgetTypes);
	WidgetTypeParameters.DocLink = TEXT("Shared/WidgetTypes");
	WidgetTypeParameters.TitleFilterString = LOCTEXT("FXsollaUIBuildersSettingsDetails_WidgetTypes", "Widget types");
	WidgetTypeParameters.TooltipText = LOCTEXT("WidgetTypes", "Edit widget types.");
	WidgetTypeParameters.TooltipExcerptName = TEXT("WidgetType");
	WidgetTypeParameters.TitleText = LOCTEXT("WidgetTypes_Menu_Description", " You can have up to 30 custom widget types for your project. \nOnce you name each type, they will show up as new type for xsolla widgets library.");

	CustomizeOneType(DetailBuilder, StaticEnum<EWidgetType>(), XsollaUIBuilderSettings->WidgetTypes, WidgetTypeParameters);
	// end of widget types

	// color types
	FTypeParametersStruct ColorTypeParameters;
	ColorTypeParameters.CategoryName = TEXT("Color types");
	ColorTypeParameters.PropertyPath = GET_MEMBER_NAME_CHECKED(UXsollaUIBuilderSettings, ColorTypes);
	ColorTypeParameters.DocLink = TEXT("Shared/ColorTypes");
	ColorTypeParameters.TitleFilterString = LOCTEXT("FXsollaUIBuildersSettingsDetails_ColorTypes", "Color types");
	ColorTypeParameters.TooltipText = LOCTEXT("ColorTypes", "Edit color types.");
	ColorTypeParameters.TooltipExcerptName = TEXT("ColorType");
	ColorTypeParameters.TitleText = LOCTEXT("ColorTypes_Menu_Description", " You can have up to 30 custom color types for your project. \nOnce you name each type, they will show up as new type for xsolla UI theme.");

	CustomizeOneType(DetailBuilder, StaticEnum<EColorType>(), XsollaUIBuilderSettings->ColorTypes, ColorTypeParameters);
	// end of color types

	// font types
	FTypeParametersStruct FontTypeParameters;
	FontTypeParameters.CategoryName = TEXT("Font types");
	FontTypeParameters.PropertyPath = GET_MEMBER_NAME_CHECKED(UXsollaUIBuilderSettings, FontTypes);
	FontTypeParameters.DocLink = TEXT("Shared/FontTypes");
	FontTypeParameters.TitleFilterString = LOCTEXT("FXsollaUIBuildersSettingsDetails_FontTypes", "Font types");
	FontTypeParameters.TooltipText = LOCTEXT("FontTypes", "Edit font types.");
	FontTypeParameters.TooltipExcerptName = TEXT("FontType");
	FontTypeParameters.TitleText = LOCTEXT("FontTypes_Menu_Description", " You can have up to 30 custom font types for your project. \nOnce you name each type, they will show up as new type for xsolla UI theme.");

	CustomizeOneType(DetailBuilder, StaticEnum<EFontType>(), XsollaUIBuilderSettings->FontTypes, FontTypeParameters);
	// end of font types

	// brush types
	FTypeParametersStruct BrushTypeParameters;
	BrushTypeParameters.CategoryName = TEXT("Brush types");
	BrushTypeParameters.PropertyPath = GET_MEMBER_NAME_CHECKED(UXsollaUIBuilderSettings, BrushTypes);
	BrushTypeParameters.DocLink = TEXT("Shared/BrushTypes");
	BrushTypeParameters.TitleFilterString = LOCTEXT("FXsollaUIBuildersSettingsDetails_BrushTypes", "Brush types");
	BrushTypeParameters.TooltipText = LOCTEXT("BrushTypes", "Edit brush types.");
	BrushTypeParameters.TooltipExcerptName = TEXT("BrushType");
	BrushTypeParameters.TitleText = LOCTEXT("BrushTypes_Menu_Description", " You can have up to 62 custom brush types for your project. \nOnce you name each type, they will show up as new type for xsolla UI theme.");

	CustomizeOneType(DetailBuilder, StaticEnum<EBrushThemeType>(), XsollaUIBuilderSettings->BrushTypes, BrushTypeParameters);
	// end of brush types
}

void FXsollaUIBuilderSettingsDetails::CustomizeOneType(IDetailLayoutBuilder& DetailBuilder, UEnum* TypeEnum, TArray<FEntityTypeName>& TypesArray, const FTypeParametersStruct& Parameters)
{
	check(TypeEnum);

	IDetailCategoryBuilder& EntityTypesCategory = DetailBuilder.EditCategory(*Parameters.CategoryName, FText::GetEmpty(), ECategoryPriority::Uncommon);
	EntityTypesCategory.InitiallyCollapsed(true);
	TSharedPtr<IPropertyHandle> EntityTypeProperty = DetailBuilder.GetProperty(Parameters.PropertyPath);

	TSharedRef<FXsollaEntityTypeList> EntityTypesListCustomization = MakeShareable(new FXsollaEntityTypeList(TypesArray, TypeEnum, EntityTypeProperty));
	EntityTypesListCustomization->UpdateConfig.BindStatic(&FXsollaUIBuilderSettingsDetails::UpdateDefaultConfigFile);

	EntityTypesListCustomization->ReloadTypes.BindLambda([&, Enum = TStrongObjectPtr<UEnum>(TypeEnum)] {
		UXsollaUIBuilderSettings::LoadType(Enum.Get(), TypesArray);
	});
	EntityTypesListCustomization->RefreshItemsList();

	const TSharedPtr<SToolTip> EntityTypesTooltip = IDocumentation::Get()->CreateToolTip(Parameters.TooltipText, nullptr, Parameters.DocLink, Parameters.TooltipExcerptName);
	EntityTypesCategory.AddCustomRow(Parameters.TitleFilterString)
	[
		SNew(STextBlock)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.ToolTip(EntityTypesTooltip)
		.AutoWrapText(true)
		.Text(Parameters.TitleText)
	];

	EntityTypesCategory.AddCustomBuilder(EntityTypesListCustomization);
}

void FXsollaUIBuilderSettingsDetails::UpdateDefaultConfigFile()
{
	UXsollaUIBuilderLibrary::GetUIBuilderSettings()->UpdateDefaultConfigFile();
}

#undef LOCTEXT_NAMESPACE
