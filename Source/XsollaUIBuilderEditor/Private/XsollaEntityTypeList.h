// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailCustomNodeBuilder.h"

struct FEntityTypeName;
class FXsollaEntityListItem;

class FXsollaEntityTypeList : public IDetailCustomNodeBuilder, public TSharedFromThis<FXsollaEntityTypeList>
{
public:
	FXsollaEntityTypeList(TArray<FEntityTypeName>& InTypeNames, UEnum* InTypeEnum, TSharedPtr<IPropertyHandle>& InTypeProperty);

	void RefreshItemsList();

	virtual void SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren) override;

	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;

	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;

	virtual void Tick(float DeltaTime) override {}
	virtual bool RequiresTick() const override { return false; }
	virtual bool InitiallyCollapsed() const { return false; }
	virtual FName GetName() const override;

private:
	void OnCommitChange();

public:
	FSimpleDelegate UpdateConfig;
	FSimpleDelegate ReloadTypes;

private:
	TArray<TSharedPtr<FXsollaEntityListItem>> ItemsList;
	TArray<FEntityTypeName>& TypeNames;
	UEnum* TypeEnum;
	TSharedPtr<IPropertyHandle> TypesProperty;
};
