// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

struct FEntityTypeName;

class FXsollaEntityListItem
{
public:
	FXsollaEntityListItem(TSharedPtr<FEntityTypeName> InTypeName)
		: TypeName(InTypeName)
	{
	}

	TSharedPtr<FEntityTypeName> TypeName;
};

struct FTypeParametersStruct
{
	FName PropertyPath;
	FString CategoryName;
	FString DocLink;
	FText TitleFilterString;
	FText TooltipText;
	FString TooltipExcerptName;
	FText TitleText;
};
