// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "XsollaEntityTypeList.h"

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
	
	UXsollaUIBuilderSettings* XsollaUIBuilderSettings;

	// widget types
	TArray<TSharedPtr<FXsollaEntityListItem>> WidgetTypeList;
	UEnum* WidgetTypeEnum;

	void UpdateDefaultConfigFile();
	
};

