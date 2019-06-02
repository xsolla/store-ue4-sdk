// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Blueprint/UserWidget.h"

#include "WebBrowser.h"

#include "XsollaStoreBrowser.generated.h"

class UXsollaStoreController;
class UWebBrowser;

UCLASS()
class XSOLLASTORE_API UXsollaStoreBrowser : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void LoadWidget();

	UWebBrowser* GetBrowser() const { return WebBrowser; }

protected:
	/** @TODO */
	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Store Browser")
	UWebBrowser* WebBrowser;
};
