// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreBrowser.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Math/NumericLimits.h"

UXsollaStoreBrowser::UXsollaStoreBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedRef<SWidget> UXsollaStoreBrowser::RebuildWidget()
{
	WebBrowser = WidgetTree->ConstructWidget<UWebBrowser>(UWebBrowser::StaticClass(), TEXT("WebBrowser"));
	UCanvasPanelSlot* WebBrowserSlot = Cast<UCanvasPanelSlot>(WebBrowser->Slot);
	if (WebBrowserSlot)
	{
		WebBrowserSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		WebBrowserSlot->SetOffsets(FMargin(0.f, 0.f));
	}

	WidgetTree->RootWidget = WebBrowser;

	return Super::RebuildWidget();
}

void UXsollaStoreBrowser::LoadWidget()
{
	AddToViewport(MAX_int32);
}
