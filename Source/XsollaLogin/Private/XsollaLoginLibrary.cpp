// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaLoginLibrary.h"

#include "XsollaLogin.h"

#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Internationalization/Regex.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "XsollaSettings/Public/XsollaProjectSettings.h"
#include "XsollaSettings/Public/XsollaSettingsModule.h"
#include "TextureResource.h"
#include "Runtime/Launch/Resources/Version.h"

UXsollaLoginLibrary::UXsollaLoginLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UXsollaLoginLibrary::IsEmailValid(const FString& EMail)
{
	const FRegexPattern EmailPattern(TEXT("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"));
	FRegexMatcher Matcher(EmailPattern, EMail);
	return Matcher.FindNext();
}

FString UXsollaLoginLibrary::GetStringCommandLineParam(const FString& ParamName)
{
	FString Result = TEXT("");
	const bool FoundValue = FParse::Value(FCommandLine::Get(), *ParamName, Result);
	return FoundValue ? FString(Result) : FString(TEXT(""));
}

FString UXsollaLoginLibrary::GetSessionTicket()
{
	IOnlineSubsystem* OnlineInterface = IOnlineSubsystem::Get();
	FString SessionTicket = OnlineInterface->GetIdentityInterface()->GetAuthToken(0);
	return SessionTicket;
}

void UXsollaLoginLibrary::LaunchPlatfromBrowser(const FString& URL)
{
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

TArray<uint8> UXsollaLoginLibrary::ConvertTextureToByteArray(UTexture2D* Texture)
{
	int Width = Texture->GetSizeX();
	int Height = Texture->GetSizeY();

	FTexturePlatformData* PlatformData = Texture->GetPlatformData();

	bool isBGRA = PlatformData->PixelFormat == PF_B8G8R8A8;

	const FColor* FormatedImageData = reinterpret_cast<const FColor*>(PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));

	TArray<FColor> colorArray;
	colorArray.SetNumZeroed(Width * Height);

	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			colorArray[Y * Width + X] = FormatedImageData[Y * Width + X];
		}
	}

	PlatformData->Mips[0].BulkData.Unlock();

	EImageCompressionQuality LocalCompressionQuality = EImageCompressionQuality::Uncompressed;

	IImageWrapperModule* ImageWrapperModule = FModuleManager::LoadModulePtr<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWriter = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);

	const size_t BitsPerPixel = (sizeof(FColor) / 4) * 8;
	const ERGBFormat SourceChannelLayout = isBGRA ? ERGBFormat::BGRA : ERGBFormat::RGBA;

	ImageWriter->SetRaw((void*)&colorArray[0], sizeof(FColor) * Width * Height, Width, Height, SourceChannelLayout, BitsPerPixel);

	const TArray64<uint8>& CompressedData = ImageWriter->GetCompressed((int32)LocalCompressionQuality);

	TArray<uint8> TextureData;
	TextureData.Append(CompressedData);

	return TextureData;
}

FString UXsollaLoginLibrary::GetUrlParameter(const FString& URL, const FString& Parameter)
{
	FString UrlOptions = URL.RightChop(URL.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));
	FString ParameterValue = UGameplayStatics::ParseOption(UrlOptions, Parameter);

	// get rid of extra symbols added by Login if any
	if (ParameterValue.Contains(TEXT("#")))
	{
		ParameterValue = ParameterValue.Left(ParameterValue.Find(TEXT("#")));
	}

	return ParameterValue;
}

FString UXsollaLoginLibrary::GetDeviceName()
{
#if PLATFORM_ANDROID
	return FAndroidMisc::GetDeviceMake() + FAndroidMisc::GetDeviceModel();
#elif PLATFORM_IOS
	return FIOSPlatformMisc::GetDefaultDeviceProfileName();
#else
	return FPlatformMisc::GetDefaultDeviceProfileName();
#endif
}

FString UXsollaLoginLibrary::GetDeviceId()
{
	return UKismetSystemLibrary::GetDeviceId();
}

FString UXsollaLoginLibrary::GetAppId()
{
	return UKismetSystemLibrary::GetGameBundleId();
}

bool UXsollaLoginLibrary::IsSteamBuildValid(FString& OutError)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	bool isValid = true;
	if (!Settings->BuildForSteam)
	{
		isValid = false;
		OutError = TEXT("Check the Build For Steam box in the settings");
	}
	else if (!FModuleManager::Get().IsModuleLoaded("OnlineSubsystemSteam"))
	{
		isValid = false;
		OutError = TEXT("Enable the OnlineSubsystemSteam plug-in");
	}
	else if (!IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM))
	{
		isValid = false;
		OutError = TEXT("Add Steam data to the Config/DefaultEngine.ini file");
	}

	return isValid;
}
