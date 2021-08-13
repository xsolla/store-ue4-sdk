// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "XsollaUIBuilderTypes.h"

class UXsollaGenericWrapper;

class FXsollaGenericWrapperDetails: public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static TSharedRef< IDetailCustomization > MakeInstance();

private:
	TSharedRef<SWidget> OnGetAllEnums();

	void OnEnumChanged(int32 InIndex);

	FText GetCurrentEnumName() const;

	bool IsEnumEditingEnabled() const;

	TArray<EWidgetType> KeyValues;

	TSharedPtr<IPropertyHandle> EnumPropertyHandle;

	TWeakObjectPtr<class UXsollaGenericWrapper> SelectedWrapper;

};




