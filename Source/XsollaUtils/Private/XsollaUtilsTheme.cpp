// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsTheme.h"

UXsollaUtilsTheme::UXsollaUtilsTheme(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FColor UXsollaUtilsTheme::GetColor(const FString& ColorName) const
{
	if (Colors.Contains(ColorName))
	{
		return Colors[ColorName];
	}

	return FColor();
}
