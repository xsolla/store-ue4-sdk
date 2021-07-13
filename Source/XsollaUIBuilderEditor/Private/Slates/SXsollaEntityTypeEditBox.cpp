// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "SXsollaEntityTypeEditBox.h"

#include "DetailLayoutBuilder.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "SXsollaEntityTypeEditBox"

void SXsollaEntityTypeEditBox::Construct(const FArguments& InArgs)
{
	Name = InArgs._Name;
	EntityTypeEnum = InArgs._EntityTypeEnum;
	OnCommitChange = InArgs._OnCommitChange;
	check(Name != nullptr && EntityTypeEnum);

	ChildSlot
	[
		SAssignNew(NameEditBox, SEditableTextBox)
		.Text(this, &SXsollaEntityTypeEditBox::GetName)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.OnTextCommitted(this, &SXsollaEntityTypeEditBox::NewNameEntered)
		.OnTextChanged(this, &SXsollaEntityTypeEditBox::OnTextChanged)
		.IsReadOnly(InArgs._TypeAsInt == 0)
		.SelectAllTextWhenFocused(true)
	];
}

void SXsollaEntityTypeEditBox::OnTextChanged(const FText& NewText)
{
	const FString NewName = NewText.ToString();

	if (NewName.Find(TEXT(" ")) != INDEX_NONE)
	{
		NameEditBox->SetError(TEXT("No white space is allowed"));
	}
	else
	{
		NameEditBox->SetError(TEXT(""));
	}
}

void SXsollaEntityTypeEditBox::NewNameEntered(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if ((CommitInfo == ETextCommit::OnEnter) || (CommitInfo == ETextCommit::OnUserMovedFocus))
	{
		FString NewName = NewText.ToString();
		if (NewName.Find(TEXT(" ")) == INDEX_NONE)
		{
			FName NewTypeName(*NewName);
			if (*Name != NAME_None && NewTypeName == NAME_None)
			{
				if (FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("SEntityTypeEditBox_DeleteConfirm", "Would you like to delete the name? If this type is used, it will invalidate the usage.")) == EAppReturnType::No)
				{
					return;
				}
			}
			if (NewTypeName != *Name)
			{
				*Name = NewTypeName;
				OnCommitChange.ExecuteIfBound();
			}
		}
		else
		{
			// clear error
			NameEditBox->SetError(TEXT(""));
		}
	}
}

FText SXsollaEntityTypeEditBox::GetName() const
{
	return FText::FromName(*Name);
}

#undef LOCTEXT_NAMESPACE