// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderWidgetsLibrary.h"

TSubclassOf<UXsollaGenericPrimitive> UXsollaUIBuilderWidgetsLibrary::GetWidgetByType(const TEnumAsByte<EWidgetType> WidgetType) const
{
	if (Widgets.Contains(WidgetType))
	{
		return Widgets[WidgetType];
	}

	return nullptr;
}