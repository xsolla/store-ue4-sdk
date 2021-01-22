// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"

class XsollaUtilsStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();

private:
	static TSharedRef<class FSlateStyleSet> Create();
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};
