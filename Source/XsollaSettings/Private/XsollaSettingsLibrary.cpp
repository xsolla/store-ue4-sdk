// // Copyright 2022 Xsolla Inc. All Rights Reserved.


#include "XsollaSettingsLibrary.h"

#include "XsollaSettingsModule.h"

UXsollaSettings* UXsollaSettingsLibrary::GetXsollaSettings()
{
	return FXsollaSettingsModule::Get().GetSettings();
}