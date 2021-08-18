// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaEntityTypeList.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Misc/MessageDialog.h"
#include "Slates/SXsollaEntityTypeEditBox.h"
#include "Widgets/Text/STextBlock.h"
#include "XsollaUIBuilderEditorTypes.h"
#include "XsollaUIBuilderTypes.h"

#define LOCTEXT_NAMESPACE "XsollaUIBuilderEntityTypeList"

FXsollaEntityTypeList::FXsollaEntityTypeList(TArray<FEntityTypeName>& InTypeNames, UEnum* InTypeEnum, TSharedPtr<IPropertyHandle>& InTypeProperty)
	: TypeNames(InTypeNames)
	, TypeEnum(InTypeEnum)
	, TypesProperty(InTypeProperty)
{
	TypesProperty->MarkHiddenByCustomization();
}

void FXsollaEntityTypeList::RefreshItemsList()
{
	//make sure no duplicate exists
	//if exists, use the last one
	for (auto Iter = TypeNames.CreateIterator(); Iter; ++Iter)
	{
		for (auto InnerIter = Iter + 1; InnerIter; ++InnerIter)
		{
			// see if same type
			if (Iter->TypeAsInt == InnerIter->TypeAsInt)
			{
				// remove the current one
				TypeNames.RemoveAt(Iter.GetIndex());
				--Iter;
				break;
			}
		}
	}

	TArray<bool> bCreatedItem;
	bCreatedItem.SetNum(TypeEnum->GetMaxEnumValue());

	ItemsList.Empty();

	// add the first one by default
	{
		bCreatedItem[0] = true;
		ItemsList.Add(MakeShareable(new FXsollaEntityListItem(MakeShareable(new FEntityTypeName(0, TEXT("Default"))))));
	}

	// we don't create the first one. First one is always default.
	for (auto Iter = TypeNames.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->TypeAsInt >= TypeEnum->NumEnums() || Iter->TypeAsInt <= 0)
		{
			continue;
		}
		bCreatedItem[Iter->TypeAsInt] = true;
		ItemsList.Add(MakeShareable(new FXsollaEntityListItem(MakeShareable(new FEntityTypeName(*Iter)))));
	}

	for (int32 Index = 0; Index < TypeEnum->GetMaxEnumValue(); ++Index)
	{
		if (bCreatedItem[Index] == false)
		{
			FEntityTypeName NeweElement(Index, TEXT(""));
			ItemsList.Add(MakeShareable(new FXsollaEntityListItem(MakeShareable(new FEntityTypeName(NeweElement)))));
		}
	}

	// sort ItemsList by Type

	ItemsList.Sort([](const TSharedPtr<FXsollaEntityListItem> A, const TSharedPtr<FXsollaEntityListItem> B) {
		check(A.IsValid());
		check(B.IsValid());
		return A->TypeName->TypeAsInt < B->TypeName->TypeAsInt;
	});

	ReloadTypes.ExecuteIfBound();
}

void FXsollaEntityTypeList::SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren)
{
}

void FXsollaEntityTypeList::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
}

void FXsollaEntityTypeList::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	FText SearchString = LOCTEXT("FXsollaEntityTypeList_EntityType", "Entity type");

	for (TSharedPtr<FXsollaEntityListItem>& Item : ItemsList)
	{
		FDetailWidgetRow& Row = ChildrenBuilder.AddCustomRow(SearchString);

		FString TypeString = TypeEnum->GetNameStringByValue(static_cast<int64>(Item->TypeName->TypeAsInt));

		Row.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TypeString))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

		Row.ValueContent()
		[
			SNew(SXsollaEntityTypeEditBox)
			.TypeAsInt(Item->TypeName->TypeAsInt)
			.Name(&Item->TypeName->Name)
			.EntityTypeEnum(TypeEnum)
			.OnCommitChange(this, &FXsollaEntityTypeList::OnCommitChange)
		];
	}
}

FName FXsollaEntityTypeList::GetName() const
{
	static const FName Name(TEXT("EntityTypesList"));
	return Name;
}

void FXsollaEntityTypeList::OnCommitChange()
{
	bool bDoCommit = true;
	//make sure it verifies all data is correct
	//skip the first one
	for (auto Iter = ItemsList.CreateConstIterator() + 1; Iter; ++Iter)
	{
		TSharedPtr<FXsollaEntityListItem> ListItem = *Iter;
		if (ListItem->TypeName->Name != NAME_None)
		{
			// make sure no same name exists
			for (auto InnerIter = Iter + 1; InnerIter; ++InnerIter)
			{
				TSharedPtr<FXsollaEntityListItem> InnerItem = *InnerIter;
				if (ListItem->TypeName->Name == InnerItem->TypeName->Name)
				{
					// duplicate name, warn user and get out
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FXsollaEntityTypeList_InvalidName", "Duplicate name found."));
					bDoCommit = false;
					break;
				}
			}
		}
	}

	if (bDoCommit)
	{
		TypesProperty->NotifyPreChange();

		TypeNames.Empty();
		for (auto Iter = ItemsList.CreateConstIterator() + 1; Iter; ++Iter)
		{
			TSharedPtr<FXsollaEntityListItem> ListItem = *Iter;
			if (ListItem->TypeName->Name != NAME_None)
			{
				TypeNames.Add(FEntityTypeName(ListItem->TypeName->TypeAsInt, ListItem->TypeName->Name));
			}
		}

		TypesProperty->NotifyPostChange();
		UpdateConfig.ExecuteIfBound();
		ReloadTypes.ExecuteIfBound();
	}
}

#undef LOCTEXT_NAMESPACE