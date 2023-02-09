// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaSettingsTypes.generated.h"

/** Target platform name */
UENUM(BlueprintType)
enum class EXsollaPublishingPlatform : uint8
{
	undefined UMETA(DisplayName = "Undefined"),
	playstation_network UMETA(DisplayName = "PlaystationNetwork"),
	xbox_live UMETA(DisplayName = "XboxLive"),
	xsolla UMETA(DisplayName = "Xsolla"),
	pc_standalone UMETA(DisplayName = "PcStandalone"),
	nintendo_shop UMETA(DisplayName = "NintendoShop"),
	google_play UMETA(DisplayName = "GooglePlay"),
	app_store_ios UMETA(DisplayName = "AppStoreIos"),
	android_standalone UMETA(DisplayName = "AndroidStandalone"),
	ios_standalone UMETA(DisplayName = "IosStandalone"),
	android_other UMETA(DisplayName = "AndroidOther"),
	ios_other UMETA(DisplayName = "IosOther"),
	pc_other UMETA(DisplayName = "PcOther")
};

/** User interface size for the payment interface. */
UENUM(BlueprintType)
enum class EXsollaPaymentUiSize : uint8
{
	small UMETA(DisplayName = "Small"),
	medium UMETA(DisplayName = "Medium"),
	large UMETA(DisplayName = "Large")
};

/** User interface theme for the payment interface. */
UENUM(BlueprintType)
enum class EXsollaPaymentUiTheme : uint8
{
	default_light UMETA(DisplayName = "Default"),
	default_dark UMETA(DisplayName = "Default Dark"),
	dark UMETA(DisplayName = "Dark"),
	ps4_default_light UMETA(DisplayName = "PS4 Default Light"),
	ps4_default_dark UMETA(DisplayName = "PS4 Default Dark")
};

/** Device type used to display the payment interface. */
UENUM(BlueprintType)
enum class EXsollaPaymentUiVersion : uint8
{
	not_specified UMETA(DisplayName = "Not specified"),
	desktop UMETA(DisplayName = "Desktop"),
	mobile UMETA(DisplayName = "Mobile")
};

/** Payment status that triggers user redirect to the return URL. */
UENUM(BlueprintType)
enum class EXsollaPaymentRedirectCondition : uint8
{
	none UMETA(DisplayName = "None"),
	successful UMETA(DisplayName = "Successful payment"),
	successful_or_canceled UMETA(DisplayName = "Successful or canceled payment"),
	any UMETA(DisplayName = "Any payment")
};

/** Payment status that triggers the display of a button clicking which redirects the user to the return URL. */
UENUM(BlueprintType)
enum class EXsollaPaymentRedirectStatusManual : uint8
{
	none UMETA(DisplayName = "None"),
	vc UMETA(DisplayName = "Purchase using virtual currency"),
	successful UMETA(DisplayName = "Successful payment"),
	successful_or_canceled UMETA(DisplayName = "Successful or canceled payment"),
	any UMETA(DisplayName = "Any payment")
};

