// Copyright 2023 Xsolla Inc. All Rights Reserved.

#include "XsollaDemoGameModeBase.h"

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetLoginDemo() const
{
	return LoginDemo;
}

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetStoreDemo() const
{
	return StoreDemo;
}
