// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderTheme.h"

#include "XsollaUIBuilderTypes.h"

FColor UXsollaUIBuilderTheme::GetColorByType(const EColorType ColorType) const
{
	if (ColorsMap.Contains(ColorType))
	{
		return ColorsMap[ColorType];
	}

	return FColor();
}

FSlateFontInfo UXsollaUIBuilderTheme::GetFontByType(const EFontType FontType) const
{
	if (FontsMap.Contains(FontType))
	{
		return FontsMap[FontType];
	}

	return FSlateFontInfo();
}

FSlateBrush UXsollaUIBuilderTheme::GetBrushByType(const EBrushThemeType BrushType) const
{
	if (BrushesMap.Contains(BrushType))
	{
		return BrushesMap[BrushType];
	}

	return FSlateBrush();
}