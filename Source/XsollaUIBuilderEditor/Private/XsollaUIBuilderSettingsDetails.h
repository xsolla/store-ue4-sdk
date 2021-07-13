// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"

struct FEntityTypeName;
class UXsollaUIBuilderSettings;
struct FTypeParametersStruct;

class FXsollaUIBuilderSettingsDetails: public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static TSharedRef< IDetailCustomization > MakeInstance();

private:
	
	UXsollaUIBuilderSettings* XsollaUIBuilderSettings;

	static void CustomizeOneType(IDetailLayoutBuilder& DetailBuilder, UEnum* TypeEnum, TArray<FEntityTypeName>& TypesArray, const FTypeParametersStruct& Parameters);
	static void UpdateDefaultConfigFile();

};

