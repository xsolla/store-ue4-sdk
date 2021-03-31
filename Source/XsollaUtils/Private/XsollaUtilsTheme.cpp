// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsTheme.h"

FColor UXsollaUtilsTheme::GetColor(const FString& ColorName) const
{
	if (Colors.Contains(ColorName))
	{
		return Colors[ColorName];
	}

	return FColor();
}

FSlateFontInfo UXsollaUtilsTheme::GetFont(const FString& FontName) const
{
	if (Fonts.Contains(FontName))
	{
		return Fonts[FontName];
	}

	return FSlateFontInfo();
}

FSlateBrush UXsollaUtilsTheme::GetBrush(const FString& BrushName) const
{
	if (Brushes.Contains(BrushName))
	{
		return Brushes[BrushName];
	}

	return FSlateBrush();
}

