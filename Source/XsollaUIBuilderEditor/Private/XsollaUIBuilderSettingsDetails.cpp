// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderSettingsDetails.h"

#include "IDocumentation.h"
#include "PropertyEditing.h"
#include "XsollaUIBuilderSettings.h"
#include "Widgets/SToolTip.h"
#include "WidgetTypeList.h"
#include "DetailLayoutBuilder.h"

#define LOCTEXT_NAMESPACE "XsollaUIBuilderSettingsDetails"

TSharedRef< IDetailCustomization > FXsollaUIBuilderSettingsDetails::MakeInstance()
{
	return MakeShareable(new FXsollaUIBuilderSettingsDetails);
}

void FXsollaUIBuilderSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& WidgetTypesCategory = DetailBuilder.EditCategory("Widget types", FText::GetEmpty(), ECategoryPriority::Uncommon);

	XsollaUIBuilderSettings = UXsollaUIBuilderSettings::Get();
	check(XsollaUIBuilderSettings);

	WidgetTypeEnum = StaticEnum<EWidgetType>();
	check(WidgetTypeEnum);

	TSharedPtr<IPropertyHandle> WidgetTypeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UXsollaUIBuilderSettings, WidgetTypes));

	TSharedRef<FWidgetTypeList> WidgetTypesListCustomization = MakeShareable(new FWidgetTypeList(XsollaUIBuilderSettings, WidgetTypeEnum, WidgetTypeProperty));
	WidgetTypesListCustomization->RefreshWidgetTypesList();

	const FString WidgetTypeDocLink = TEXT("Shared/WidgetTypes");
	TSharedPtr<SToolTip> WidgetTypesTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("WidgetTypes", "Edit widget types."), NULL, WidgetTypeDocLink, TEXT("WidgetType"));
	// Customize collision section
	WidgetTypesCategory.AddCustomRow(LOCTEXT("FXsollaUIBuildersSettingsDetails_WidgetTypes", "Widget types"))
		[SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.ToolTip(WidgetTypesTooltip)
				.AutoWrapText(true)
				.Text(LOCTEXT("WidgetTypes_Menu_Description", " You can have up to 30 custom widget types for your project. \nOnce you name each type, they will show up as new type for xsolla widgets library."))];

	WidgetTypesCategory.AddCustomBuilder(WidgetTypesListCustomization);
}


#undef LOCTEXT_NAMESPACE