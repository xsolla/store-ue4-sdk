// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "XsollaEntityTypeList.h"

class UXsollaUIBuilderSettings;
class UPhysicsSettings;

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

class FXsollaUIBuilderSettingsDetails: public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//

	static TSharedRef< IDetailCustomization > MakeInstance();

private:
	
	UXsollaUIBuilderSettings* XsollaUIBuilderSettings;

	static void CustomizeOneType(IDetailLayoutBuilder& DetailBuilder, UEnum* TypeEnum, TArray<FEntityTypeName>& TypesArray, const FTypeParametersStruct& Parameters);
	static void UpdateDefaultConfigFile();

};

