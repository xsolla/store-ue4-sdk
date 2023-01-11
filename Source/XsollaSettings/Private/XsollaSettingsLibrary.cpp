// // Copyright 2023 Xsolla Inc. All Rights Reserved.


#include "XsollaSettingsLibrary.h"

#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"

UXsollaProjectSettings* UXsollaSettingsLibrary::GetProjectSettings()
{
	return FXsollaSettingsModule::Get().GetSettings();
}