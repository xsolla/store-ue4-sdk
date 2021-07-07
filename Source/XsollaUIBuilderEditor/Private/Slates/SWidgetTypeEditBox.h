#pragma once
#include "Widgets/Input/SEditableTextBox.h"
struct FWidgetTypeName;

DECLARE_DELEGATE(FOnCommitChange)

class SWidgetTypeEditBox : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SWidgetTypeEditBox) { }
		SLATE_ARGUMENT(TSharedPtr<FWidgetTypeName>, WidgetType)
		SLATE_ARGUMENT(UEnum*, WidgetTypeEnum)
		SLATE_EVENT(FOnCommitChange, OnCommitChange)
	SLATE_END_ARGS()

public:

	/**
	* Constructs the application.
	*
	* @param InArgs - The Slate argument list.
	*/
	void Construct(const FArguments& InArgs);
	
	FString GetTypeString() const;

	FText GetName() const;
	void NewNameEntered(const FText& NewText, ETextCommit::Type CommitInfo);
	void OnTextChanged(const FText& NewText);

private:
	TSharedPtr<FWidgetTypeName>			WidgetType;
	UEnum*								WidgetTypeEnum;
	FOnCommitChange						OnCommitChange;
	TSharedPtr<SEditableTextBox>		NameEditBox;
};
