#include "SWidgetTypeEditBox.h"

#include "DetailLayoutBuilder.h"
#include "XsollaUIBuilderSettings.h"
#include "XsollaUIBuilderTypes.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "SWidgetTypeEditBox"

void SWidgetTypeEditBox::Construct(const FArguments& InArgs)
{
	WidgetType = InArgs._WidgetType;
	WidgetTypeEnum = InArgs._WidgetTypeEnum;
	OnCommitChange = InArgs._OnCommitChange;
	check(WidgetType.IsValid() && WidgetTypeEnum);

	ChildSlot
		[SAssignNew(NameEditBox, SEditableTextBox)
				.Text(this, &SWidgetTypeEditBox::GetName)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.OnTextCommitted(this, &SWidgetTypeEditBox::NewNameEntered)
				.OnTextChanged(this, &SWidgetTypeEditBox::OnTextChanged)
				.IsReadOnly(WidgetType->Type == WidgetType_Default)
				.SelectAllTextWhenFocused(true)];
}

void SWidgetTypeEditBox::OnTextChanged(const FText& NewText)
{
	FString NewName = NewText.ToString();

	if (NewName.Find(TEXT(" ")) != INDEX_NONE)
	{
		// no white space
		NameEditBox->SetError(TEXT("No white space is allowed"));
	}
	else
	{
		NameEditBox->SetError(TEXT(""));
	}
}

void SWidgetTypeEditBox::NewNameEntered(const FText& NewText, ETextCommit::Type CommitInfo)
{
	// Don't digest the number if we just clicked away from the pop-up
	if ((CommitInfo == ETextCommit::OnEnter) || (CommitInfo == ETextCommit::OnUserMovedFocus))
	{
		FString NewName = NewText.ToString();
		if (NewName.Find(TEXT(" ")) == INDEX_NONE)
		{
			FName NewWidgetName(*NewName);
			if (WidgetType->Name != NAME_None && NewWidgetName == NAME_None)
			{
				if (FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("SWidgetTypeListItem_DeleteConfirm", "Would you like to delete the name? If this type is used, it will invalidate the usage.")) == EAppReturnType::No)
				{
					return;
				}
			}
			if (NewWidgetName != WidgetType->Name)
			{
				WidgetType->Name = NewWidgetName;
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

FText SWidgetTypeEditBox::GetName() const
{
	return FText::FromName(WidgetType->Name);
}

#undef LOCTEXT_NAMESPACE