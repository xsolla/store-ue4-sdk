// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FXsollaUtilsCommands : public TCommands<FXsollaUtilsCommands>
{
public:

	/** Default constructor. */
	FXsollaUtilsCommands();

	/** Initialize commands */
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenThemeEditorCommand;
};
