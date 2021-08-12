// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaWorldObject.h"

#include "Blueprint/UserWidget.h"

UWorld* UXsollaWorldObject::GetWorld() const
{
	if (Owner != nullptr)
	{
		return Owner->GetWorld();
	}

	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr;
	}
	else if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	else
	{
		return nullptr;
	}
}