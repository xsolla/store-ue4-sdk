// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/** Custom UE editor commands. */
class FXsollaUtilsCommands : public TCommands<FXsollaUtilsCommands>
{
public:
	
	FXsollaUtilsCommands();

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenThemeEditorCommand;
};
