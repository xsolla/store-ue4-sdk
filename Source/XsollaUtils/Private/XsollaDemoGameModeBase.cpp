// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaDemoGameModeBase.h"

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetLoginDemo() const
{
	return LoginDemo;
}

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetStoreDemo() const
{
	return StoreDemo;
}
