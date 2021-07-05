// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderTheme.h"

FColor UXsollaUIBuilderTheme::GetColor(const FString& ColorName) const
{
	if (Colors.Contains(ColorName))
	{
		return Colors[ColorName];
	}

	return FColor();
}

FSlateFontInfo UXsollaUIBuilderTheme::GetFont(const FString& FontName) const
{
	if (Fonts.Contains(FontName))
	{
		return Fonts[FontName];
	}

	return FSlateFontInfo();
}

FSlateBrush UXsollaUIBuilderTheme::GetBrush(const FString& BrushName) const
{
	if (Brushes.Contains(BrushName))
	{
		return Brushes[BrushName];
	}

	return FSlateBrush();
}

