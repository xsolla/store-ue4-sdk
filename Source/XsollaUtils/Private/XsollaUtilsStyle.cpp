// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsStyle.h"
#include "XsollaUtilsModule.h"

#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> XsollaUtilsStyle::StyleInstance = NULL;

void XsollaUtilsStyle::Initialize()
{
    if (!StyleInstance.IsValid())
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

void XsollaUtilsStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
    ensure(StyleInstance.IsUnique());
    StyleInstance.Reset();
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > XsollaUtilsStyle::Create()
{
    TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("XsollaUtilsStyle"));
    Style->SetContentRoot(IPluginManager::Get().FindPlugin("Xsolla")->GetBaseDir() / TEXT("Content/Utils"));

    Style->Set("FXsollaUtilsCommands.OpenThemeEditorCommand", new IMAGE_BRUSH(TEXT("ThemeEditorIcon"), Icon32x32));

    return Style;
}

#undef IMAGE_BRUSH

void XsollaUtilsStyle::ReloadTextures()
{
    if (FSlateApplication::IsInitialized())
    {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}
