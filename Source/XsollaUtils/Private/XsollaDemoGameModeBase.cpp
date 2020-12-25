// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaDemoGameModeBase.h"

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetLoginDemo() const
{
	return LoginDemo;
}

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetStoreDemo() const
{
	return StoreDemo;
}
