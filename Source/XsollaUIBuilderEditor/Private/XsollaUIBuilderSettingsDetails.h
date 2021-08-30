// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"

class UXsollaUIBuilderSettings;

struct FEntityTypeName;
struct FTypeParametersStruct;

class FXsollaUIBuilderSettingsDetails : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static TSharedRef<IDetailCustomization> MakeInstance();

private:
	UXsollaUIBuilderSettings* XsollaUIBuilderSettings;

	static void CustomizeOneType(IDetailLayoutBuilder& DetailBuilder, UEnum* TypeEnum, TArray<FEntityTypeName>& TypesArray, const FTypeParametersStruct& Parameters);
	static void UpdateDefaultConfigFile();
};
