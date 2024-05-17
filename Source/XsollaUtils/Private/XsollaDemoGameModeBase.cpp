// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaDemoGameModeBase.h"

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetLoginDemo() const
{
	return LoginDemo;
}

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetStoreDemo() const
{
	return StoreDemo;
}
