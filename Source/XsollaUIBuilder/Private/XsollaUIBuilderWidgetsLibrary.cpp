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
