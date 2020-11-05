// Fill out your copyright notice in the Description page of Project Settings.


#include "XsollaDemoGameModeBase.h"

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetLoginDemo() const
{
	return LoginDemo;
}

TSubclassOf<UUserWidget> AXsollaDemoGameModeBase::GetStoreDemo() const
{
	return StoreDemo;
}
