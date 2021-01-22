// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsModule.h"

#include "XsollaUtilsCommands.h"
#include "XsollaUtilsDefines.h"
#include "XsollaUtilsImageLoader.h"
#include "XsollaUtilsSettings.h"
#include "XsollaUtilsStyle.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"
#include "Editor/Blutility/Public/EditorUtilitySubsystem.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FXsollaUtilsModule"

void FXsollaUtilsModule::StartupModule()
{
	XsollaUtilsStyle::Initialize();
	XsollaUtilsStyle::ReloadTextures();

	FXsollaUtilsCommands::Register();

	XsollaUtilsEditorCommands = MakeShareable(new FUICommandList);

	XsollaUtilsEditorCommands->MapAction(
		FXsollaUtilsCommands::Get().OpenThemeEditorCommand,
		FExecuteAction::CreateRaw(this, &FXsollaUtilsModule::OpenThemeEditor),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, XsollaUtilsEditorCommands, FMenuExtensionDelegate::CreateRaw(this, &FXsollaUtilsModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, XsollaUtilsEditorCommands, FToolBarExtensionDelegate::CreateRaw(this, &FXsollaUtilsModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	ImageLoader = NewObject<UXsollaUtilsImageLoader>();
	ImageLoader->AddToRoot();

	XsollaUtilsSettings = NewObject<UXsollaUtilsSettings>(GetTransientPackage(), "XsollaUtilsSettings", RF_Standalone);
	XsollaUtilsSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaUtils",
			LOCTEXT("RuntimeSettingsName", "Xsolla Utils"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Utils"),
			XsollaUtilsSettings);
	}
}

void FXsollaUtilsModule::ShutdownModule()
{
	XsollaUtilsStyle::Shutdown();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaUtils");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		ImageLoader->RemoveFromRoot();
		XsollaUtilsSettings->RemoveFromRoot();
	}
	else
	{
		ImageLoader = nullptr;
		XsollaUtilsSettings = nullptr;
	}
}

void FXsollaUtilsModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FXsollaUtilsCommands::Get().OpenThemeEditorCommand);
}

void FXsollaUtilsModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FXsollaUtilsCommands::Get().OpenThemeEditorCommand);
}

UXsollaUtilsImageLoader* FXsollaUtilsModule::GetImageLoader()
{
	return ImageLoader;
}

UXsollaUtilsSettings* FXsollaUtilsModule::GetSettings() const
{
	check(XsollaUtilsSettings);
	return XsollaUtilsSettings;
}

void FXsollaUtilsModule::OpenThemeEditor()
{
	const FString ThemeEditorWidgetPath = "EditorUtilityWidgetBlueprint'/Xsolla/Utils/XsollaThemeEditor'";
	FSoftObjectPath ThemeEditorWidgetObjectPath = FSoftObjectPath(ThemeEditorWidgetPath);
	UObject* ThemeEditorWidgetObject = ThemeEditorWidgetObjectPath.TryLoad();
	UWidgetBlueprint* ThemeEditorWidgetBlueprint = Cast<UWidgetBlueprint>(ThemeEditorWidgetObject);

	if (!ThemeEditorWidgetBlueprint || ThemeEditorWidgetBlueprint->IsPendingKillOrUnreachable())
	{
		return;
	}

	if (!ThemeEditorWidgetBlueprint->GeneratedClass->IsChildOf(UEditorUtilityWidget::StaticClass()))
	{
		return;
	}

	UEditorUtilityWidgetBlueprint* ThemeEditorWidget = (UEditorUtilityWidgetBlueprint*)ThemeEditorWidgetBlueprint;
	if (ThemeEditorWidget)
	{
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		EditorUtilitySubsystem->SpawnAndRegisterTab(ThemeEditorWidget);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaUtilsModule, XsollaUtils);

DEFINE_LOG_CATEGORY(LogXsollaUtils);