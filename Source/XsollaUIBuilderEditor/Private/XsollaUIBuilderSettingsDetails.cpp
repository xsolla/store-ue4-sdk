// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderSettingsDetails.h"

#include "IDocumentation.h"
#include "PropertyEditing.h"
#include "XsollaUIBuilderSettings.h"
#include "Widgets/SToolTip.h"
#include "XsollaEntityTypeList.h"
#include "DetailLayoutBuilder.h"
#include "XsollaUIBuilderTypes.h"

#define LOCTEXT_NAMESPACE "XsollaUIBuilderSettingsDetails"

TSharedRef< IDetailCustomization > FXsollaUIBuilderSettingsDetails::MakeInstance()
{
	return MakeShareable(new FXsollaUIBuilderSettingsDetails);
}

void FXsollaUIBuilderSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	XsollaUIBuilderSettings = UXsollaUIBuilderSettings::Get();
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
}

void FXsollaUIBuilderSettingsDetails::CustomizeOneType(IDetailLayoutBuilder& DetailBuilder, UEnum* TypeEnum, TArray<FEntityTypeName>& TypesArray, const FTypeParametersStruct& Parameters)
{
	check(TypeEnum);
	
	IDetailCategoryBuilder& EntityTypesCategory = DetailBuilder.EditCategory(*Parameters.CategoryName, FText::GetEmpty(), ECategoryPriority::Important);
	TSharedPtr<IPropertyHandle> EntityTypeProperty = DetailBuilder.GetProperty(Parameters.PropertyPath);

	TSharedRef<FXsollaEntityTypeList> EntityTypesListCustomization = MakeShareable(new FXsollaEntityTypeList(TypesArray, TypeEnum, EntityTypeProperty));
	EntityTypesListCustomization->UpdateConfig.BindStatic(&FXsollaUIBuilderSettingsDetails::UpdateDefaultConfigFile);

	EntityTypesListCustomization->ReloadTypes.BindLambda([&] {
		UXsollaUIBuilderSettings::LoadType(TypeEnum, TypesArray);
	});
	EntityTypesListCustomization->RefreshItemsList();

	const TSharedPtr<SToolTip> EntityTypesTooltip = IDocumentation::Get()->CreateToolTip(Parameters.TooltipText, NULL, Parameters.DocLink, Parameters.TooltipExcerptName);
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
	UXsollaUIBuilderSettings::Get()->UpdateDefaultConfigFile();
}

#undef LOCTEXT_NAMESPACE