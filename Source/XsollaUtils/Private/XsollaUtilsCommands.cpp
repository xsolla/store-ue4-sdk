// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsCommands.h"

#define LOCTEXT_NAMESPACE "FXsollaUtilsCommands"

FXsollaUtilsCommands::FXsollaUtilsCommands()
	: TCommands<FXsollaUtilsCommands>("FXsollaUtilsCommands", LOCTEXT("XsollaUtils", "Xsolla Utils Editor"), NAME_None, "XsollaUtilsStyle")
{
}

void FXsollaUtilsCommands::RegisterCommands()
{
	UI_COMMAND(OpenThemeEditorCommand, "Theme Editor", "Open editor for user interface theme customization.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
