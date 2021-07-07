#pragma once
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailCustomNodeBuilder.h"
#include "Misc/MessageDialog.h"
#include "XsollaUIBuilderSettings.h"
#include "Slates/SWidgetTypeEditBox.h"

#define LOCTEXT_NAMESPACE "XsollaUIBuilderWidgetTypeList"

class FWidgetTypeListItem
{
public:
	/**
	* Constructor takes the required details
	*/
	FWidgetTypeListItem(TSharedPtr<FWidgetTypeName> InWidgetType)
		: WidgetType(InWidgetType)
	{
	}

	TSharedPtr<FWidgetTypeName> WidgetType;
};

class FWidgetTypeList : public IDetailCustomNodeBuilder, public TSharedFromThis<FWidgetTypeList>
{
public:
	FWidgetTypeList(UXsollaUIBuilderSettings* InSettings, UEnum* InWidgetTypeEnum, TSharedPtr<IPropertyHandle>& InWidgetTypeProperty )
		: Settings(InSettings)
		, WidgetTypeEnum(InWidgetTypeEnum)
		, WidgetTypesProperty(InWidgetTypeProperty)
	{
		WidgetTypesProperty->MarkHiddenByCustomization();
	}

	void RefreshWidgetTypesList()
	{
		 //make sure no duplicate exists
		 //if exists, use the last one
		for (auto Iter = Settings->WidgetTypes.CreateIterator(); Iter; ++Iter)
		 {
			 for(auto InnerIter = Iter+1; InnerIter; ++InnerIter)
			 {
				 // see if same type
				 if(Iter->Type == InnerIter->Type)
				 {
					 // remove the current one
					 Settings->WidgetTypes.RemoveAt(Iter.GetIndex());
					 --Iter;
					 break;
				 }
			 }
		 }

		 bool bCreatedItem[WidgetType_Max];
		 FGenericPlatformMemory::Memzero(bCreatedItem, sizeof(bCreatedItem));

		 WidgetTypesList.Empty();

		 // add the first one by default
		 {
			 bCreatedItem[0] = true;
			 WidgetTypesList.Add(MakeShareable(new FWidgetTypeListItem(MakeShareable(new FWidgetTypeName(WidgetType_Default, TEXT("Default"))))));
		 }

		 // we don't create the first one. First one is always default. 
		 for(auto Iter = Settings->WidgetTypes.CreateIterator(); Iter; ++Iter)
		 {
			 bCreatedItem[Iter->Type] = true;
			 WidgetTypesList.Add(MakeShareable(new FWidgetTypeListItem(MakeShareable(new FWidgetTypeName(*Iter)))));
		 }

		 for(int32 Index = (int32)WidgetType1; Index < WidgetType_Max; ++Index)
		 {
			 if(bCreatedItem[Index] == false)
			 {
				 FWidgetTypeName NeweElement((EWidgetType)Index, TEXT(""));
				 WidgetTypesList.Add(MakeShareable(new FWidgetTypeListItem(MakeShareable(new FWidgetTypeName(NeweElement)))));
			 }
		 }

		 // sort WidgetTypeList by Type

		 struct FCompareWidgetType
		 {
			 FORCEINLINE bool operator()(const TSharedPtr<FWidgetTypeListItem> A, const TSharedPtr<FWidgetTypeListItem> B) const
			 {
				 check(A.IsValid());
				 check(B.IsValid());
				 return A->WidgetType->Type < B->WidgetType->Type;
			 }
		 };

		 WidgetTypesList.Sort(FCompareWidgetType());

		 Settings->LoadWidgetType();

		 RegenerateChildren.ExecuteIfBound();
	}

	virtual void SetOnRebuildChildren( FSimpleDelegate InOnRegenerateChildren ) override
	{
		RegenerateChildren = InOnRegenerateChildren;
	}

	virtual void GenerateHeaderRowContent( FDetailWidgetRow& NodeRow ) override
	{
		// no header row
	}

	virtual void GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder ) override
	{
		 FText SearchString = LOCTEXT("FXsollaUIBuilderSettingsDetails_WidgetType", "Widget type");

		 for(TSharedPtr<FWidgetTypeListItem>& Item : WidgetTypesList)
		 {
			 FDetailWidgetRow& Row = ChildrenBuilder.AddCustomRow(SearchString);

			 FString TypeString = WidgetTypeEnum->GetNameStringByValue((int64)Item->WidgetType->Type);

			 Row.NameContent()
			 [
				 SNew(STextBlock)
				 .Text(FText::FromString(TypeString))
				 .Font(IDetailLayoutBuilder::GetDetailFont())
			 ];

			 Row.ValueContent()
			 [
				 SNew(SWidgetTypeEditBox)
				 .WidgetType(Item->WidgetType)
				 .WidgetTypeEnum(WidgetTypeEnum)
				 .OnCommitChange(this, &FWidgetTypeList::OnCommitChange)
			 ];
		 }
	}

	virtual void Tick( float DeltaTime ) override {}
	virtual bool RequiresTick() const override { return false; }
	virtual bool InitiallyCollapsed() const { return false; }
	virtual FName GetName() const override
	{
		static const FName Name(TEXT("WidgetTypesList"));
		return Name;
	}
private:

	void OnCommitChange()
	{
		bool bDoCommit=true;
		// make sure it verifies all data is correct
		// skip the first one
		for(auto Iter = WidgetTypesList.CreateConstIterator()+1; Iter; ++Iter)
		{
			TSharedPtr<FWidgetTypeListItem> ListItem = *Iter;
			if(ListItem->WidgetType->Name != NAME_None)
			{
				// make sure no same name exists
				for(auto InnerIter= Iter+1; InnerIter; ++InnerIter)
				{
					TSharedPtr<FWidgetTypeListItem> InnerItem = *InnerIter;
					if(ListItem->WidgetType->Name == InnerItem->WidgetType->Name)
					{
						// duplicate name, warn user and get out
						FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FXsollaUIBuilderSettingsDetails_InvalidName", "Duplicate name found."));
						bDoCommit = false;
						break;
					}
				}
			}
		}

		 if(bDoCommit)
		 {
			 WidgetTypesProperty->NotifyPreChange();

			 Settings->WidgetTypes.Empty();
			 for(auto Iter = WidgetTypesList.CreateConstIterator()+1; Iter; ++Iter)
			 {
				 TSharedPtr<FWidgetTypeListItem> ListItem = *Iter;
				 if(ListItem->WidgetType->Name != NAME_None)
				 {
					 Settings->WidgetTypes.Add(FWidgetTypeName(ListItem->WidgetType->Type, ListItem->WidgetType->Name));
				 }
			 }

			 Settings->UpdateDefaultConfigFile();
			 Settings->LoadWidgetType();

			 WidgetTypesProperty->NotifyPostChange();
		 }
	}
private:
	FSimpleDelegate RegenerateChildren;
	TArray< TSharedPtr< FWidgetTypeListItem > >	WidgetTypesList;
	UXsollaUIBuilderSettings* Settings;
	UEnum* WidgetTypeEnum;
	TSharedPtr<IPropertyHandle> WidgetTypesProperty;
};

#undef LOCTEXT_NAMESPACE