// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderTheme.h"

#include "XsollaUIBuilderTypes.h"

FColor UXsollaUIBuilderTheme::GetColor(const FString& ColorName) const
{
	if (Colors.Contains(ColorName))
	{
		return Colors[ColorName];
	}

	return FColor();
}

FColor UXsollaUIBuilderTheme::GetColorByType(const EColorType ColorType) const
{
	if (ColorsMap.Contains(ColorType))
	{
		return ColorsMap[ColorType];
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

FSlateFontInfo UXsollaUIBuilderTheme::GetFontByType(const EFontType FontType) const
{
	if (FontsMap.Contains(FontType))
	{
		return FontsMap[FontType];
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

FSlateBrush UXsollaUIBuilderTheme::GetBrushByType(const EBrushThemeType BrushType) const
{
	if (BrushesMap.Contains(BrushType))
	{
		return BrushesMap[BrushType];
	}

	return FSlateBrush();
}