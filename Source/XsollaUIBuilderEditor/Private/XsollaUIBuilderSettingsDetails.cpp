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
	IDetailCategoryBuilder& WidgetTypesCategory = DetailBuilder.EditCategory("Widget types", FText::GetEmpty(), ECategoryPriority::Important);
	WidgetTypeEnum = StaticEnum<EWidgetType>();
	check(WidgetTypeEnum);

	TSharedPtr<IPropertyHandle> WidgetTypeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UXsollaUIBuilderSettings, WidgetTypes));

	TSharedRef<FXsollaEntityTypeList> WidgetTypesListCustomization = MakeShareable(new FXsollaEntityTypeList(XsollaUIBuilderSettings->WidgetTypes, WidgetTypeEnum, WidgetTypeProperty));
	WidgetTypesListCustomization->UpdateConfig.BindRaw(this, &FXsollaUIBuilderSettingsDetails::UpdateDefaultConfigFile);

	WidgetTypesListCustomization->ReloadTypes.BindLambda([&]
	{
		UXsollaUIBuilderSettings::LoadType(StaticEnum<EWidgetType>(), XsollaUIBuilderSettings->WidgetTypes);
	});
	WidgetTypesListCustomization->RefreshItemsList();
	
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
	// end of widget types
}

void FXsollaUIBuilderSettingsDetails::UpdateDefaultConfigFile()
{
	XsollaUIBuilderSettings->UpdateDefaultConfigFile();
}

#undef LOCTEXT_NAMESPACE