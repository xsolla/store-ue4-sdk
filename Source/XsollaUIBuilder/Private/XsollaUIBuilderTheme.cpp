// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUIBuilderTheme.h"

#include "XsollaUIBuilderTypes.h"

FColor UXsollaUIBuilderTheme::GetColorByType(const EColorType ColorType, bool& bIsFound) const
{
	if (ColorsMap.Contains(ColorType))
	{
		bIsFound = true;
		return ColorsMap[ColorType];
	}

	return FColor();
}

FSlateFontInfo UXsollaUIBuilderTheme::GetFontByType(const EFontType FontType, bool& bIsFound) const
{
	if (FontsMap.Contains(FontType))
	{
		bIsFound = true;
		return FontsMap[FontType];
	}

	return FSlateFontInfo();
}

FSlateBrush UXsollaUIBuilderTheme::GetBrushByType(const EBrushThemeType BrushType, bool& bIsFound) const
{
	if (BrushesMap.Contains(BrushType))
	{
		bIsFound = true;
		return BrushesMap[BrushType];
	}

	return FSlateBrush();
}