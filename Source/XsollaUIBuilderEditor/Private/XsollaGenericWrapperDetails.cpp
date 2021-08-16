// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaGenericWrapperDetails.h"

#include "IDocumentation.h"
#include "PropertyEditing.h"
#include "Widgets/SToolTip.h"
#include "XsollaEntityTypeList.h"
#include "DetailLayoutBuilder.h"
#include "XsollaUIBuilderLibrary.h"
#include "XsollaUIBuilderWidgetsLibrary.h"
#include "XsollaUtilsLibrary.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/XsollaGenericWrapper.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "XsollaGenericWrapperDetails"

TSharedRef< IDetailCustomization > FXsollaGenericWrapperDetails::MakeInstance()
{
	return MakeShareable(new FXsollaGenericWrapperDetails);
}

void FXsollaGenericWrapperDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	EnumPropertyHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UXsollaGenericWrapper, WidgetLibraryType));
	EnumPropertyHandle->MarkHiddenByCustomization();
	
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

	if (SelectedObjects.Num() != 1)
	{
		return;
	}

	SelectedWrapper = Cast<UXsollaGenericWrapper>(SelectedObjects[0]);

	if (!SelectedWrapper.IsValid())
	{
		return;
	}
	
	auto WrapperInterfaces = DetailBuilder.GetBaseClass()->Interfaces;
	auto AvailablePrimitives = UXsollaUIBuilderLibrary::GetCurrentWidgetsLibrary()->Widgets;

	for (const auto WrapperInterface : WrapperInterfaces)
	{
		for (auto KeyValue : AvailablePrimitives)
		{
			if (KeyValue.Value != nullptr)
			{
				for (const auto PrimitiveInterface : KeyValue.Value->Interfaces)
				{
					if (WrapperInterface.Class == PrimitiveInterface.Class)
					{
						KeyValues.Add(KeyValue.Key);
					}
				}
			}
		}
	}

	auto& DefaultCategory = DetailBuilder.EditCategory(TEXT("Default"));
	DefaultCategory.AddCustomRow(LOCTEXT("WidgetLibraryNameEditor", "WidgetLibraryNameEditor"))
		.NameContent()
		[
			EnumPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
		SNew(SComboButton)
			.OnGetMenuContent(this, &FXsollaGenericWrapperDetails::OnGetAllEnums)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.IsEnabled(this, &FXsollaGenericWrapperDetails::IsEnumEditingEnabled)
			.ButtonContent()
				[
				SNew(STextBlock)
					.Text(this, &FXsollaGenericWrapperDetails::GetCurrentEnumName)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
		];
}

TSharedRef<SWidget> FXsollaGenericWrapperDetails::OnGetAllEnums()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	for (int32 Idx = 0; Idx < KeyValues.Num(); Idx++)
	{
		FString DisplayValue = UXsollaUtilsLibrary::GetEnumValueAsDisplayNameString("EWidgetType", KeyValues[Idx]);
		FUIAction ItemAction(FExecuteAction::CreateSP(this, &FXsollaGenericWrapperDetails::OnEnumChanged, Idx));
		MenuBuilder.AddMenuEntry(FText::FromString(DisplayValue), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void FXsollaGenericWrapperDetails::OnEnumChanged(int32 InIndex)
{
	EnumPropertyHandle->SetValue(static_cast<uint8>(KeyValues[InIndex]));
}

FText FXsollaGenericWrapperDetails::GetCurrentEnumName() const
{
	FText Result;
	EnumPropertyHandle->GetValueAsDisplayText(Result);
	return Result;
}

bool FXsollaGenericWrapperDetails::IsEnumEditingEnabled() const
{
	return SelectedWrapper.IsValid() ? SelectedWrapper.Get()->OverrideWidget == nullptr : true;
}

#undef LOCTEXT_NAMESPACE