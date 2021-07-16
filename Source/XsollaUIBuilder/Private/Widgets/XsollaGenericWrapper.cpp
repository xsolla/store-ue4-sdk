// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "Widgets/XsollaGenericWrapper.h"

#include "XsollaUIBuilderLibrary.h"
#include "XsollaUIBuilderWidgetsLibrary.h"

TSubclassOf<UUserWidget> UXsollaGenericWrapper::GetWidgetClass() const
{
	if (OverrideWidget != nullptr)
	{
		return OverrideWidget;
	}
	return UXsollaUIBuilderLibrary::GetCurrentWidgetsLibrary()->GetWidgetByType(WidgetLibraryType);
}

void UXsollaGenericWrapper::UpdateThemeParameters(const FThemeParameters& InParameters)
{
	if (UXsollaUIBuilderLibrary::IsEmpty(InParameters))
	{
		UXsollaUIBuilderLibrary::Clear(Parameters);
		return;
	}
	
	if (UXsollaUIBuilderLibrary::IsEmpty(Parameters))
	{
		Parameters = InParameters;
		return;
	}
	
	for (const auto& KeyValue : InParameters.Colors)
	{
		if(!Parameters.Colors.Contains(KeyValue.Key))
		{
			Parameters = InParameters;
			return;
		}
	}

	for (const auto& KeyValue : InParameters.Fonts)
	{
		if (!Parameters.Fonts.Contains(KeyValue.Key))
		{
			Parameters = InParameters;
			return;
		}
	}

	for (const auto& KeyValue : InParameters.Brushes)
	{
		if (!Parameters.Brushes.Contains(KeyValue.Key))
		{
			Parameters = InParameters;
			return;
		}
	}
}
