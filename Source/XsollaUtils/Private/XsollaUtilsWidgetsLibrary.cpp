// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsWidgetsLibrary.h"

TSubclassOf<UUserWidget> UXsollaUtilsWidgetsLibrary::GetWidget(const FString& WidgetName) const
{
	if (Widgets.Contains(WidgetName))
	{
		return Widgets[WidgetName];
	}

	return nullptr;
}
