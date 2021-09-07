// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once
#include "Widgets/Input/SEditableTextBox.h"

DECLARE_DELEGATE(FOnCommitChange)

class SXsollaEntityTypeEditBox : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SXsollaEntityTypeEditBox) { }
		SLATE_ARGUMENT(int32, TypeAsInt)
		SLATE_ARGUMENT(FName*, Name)
		SLATE_ARGUMENT(UEnum*, EntityTypeEnum)
		SLATE_EVENT(FOnCommitChange, OnCommitChange)
	SLATE_END_ARGS()

public:
	/**
	* Constructs the application.
	*
	* @param InArgs - The Slate argument list.
	*/
	void Construct(const FArguments& InArgs);

	FText GetName() const;
	void NewNameEntered(const FText& NewText, ETextCommit::Type CommitInfo);
	void OnTextChanged(const FText& NewText);

private:
	FName* Name;
	UEnum* EntityTypeEnum;
	FOnCommitChange OnCommitChange;
	TSharedPtr<SEditableTextBox> NameEditBox;
};
