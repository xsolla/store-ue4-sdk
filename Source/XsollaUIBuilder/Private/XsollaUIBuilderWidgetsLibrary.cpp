// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderWidgetsLibrary.h"

TSubclassOf<UUserWidget> UXsollaUIBuilderWidgetsLibrary::GetWidget(const FString& WidgetName) const
{
	if (Widgets.Contains(WidgetName))
	{
		return Widgets[WidgetName];
	}

	return nullptr;
}

TSubclassOf<UUserWidget> UXsollaUIBuilderWidgetsLibrary::GetWidgetByType(const TEnumAsByte<EWidgetType> WidgetType) const
{
	if (WidgetWrappers.Contains(WidgetType))
	{
		return WidgetWrappers[WidgetType];
	}

	return nullptr;
}