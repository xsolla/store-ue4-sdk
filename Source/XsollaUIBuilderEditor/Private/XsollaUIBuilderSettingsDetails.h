// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "WidgetTypeList.h"

class UXsollaUIBuilderSettings;
class UPhysicsSettings;

class FXsollaUIBuilderSettingsDetails: public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//

	static TSharedRef< IDetailCustomization > MakeInstance();

private:
	TArray<TSharedPtr<FWidgetTypeListItem>> WidgetTypeList;

	UXsollaUIBuilderSettings* XsollaUIBuilderSettings;
	UEnum* WidgetTypeEnum;
	// functions
	void OnCommitChange();
};

