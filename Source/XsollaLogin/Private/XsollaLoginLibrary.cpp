// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginLibrary.h"

#include "XsollaLogin.h"

#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Internationalization/Regex.h"
#include "Misc/Base64.h"
#include "Misc/CommandLine.h"
#include "Online.h"
#include "OnlineSubsystem.h"

UXsollaLoginLibrary::UXsollaLoginLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaLoginSettings* UXsollaLoginLibrary::GetLoginSettings()
{
	return FXsollaLoginModule::Get().GetSettings();
}

bool UXsollaLoginLibrary::IsEmailValid(const FString& EMail)
{
	FRegexPattern EmailPattern(TEXT("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"));
	FRegexMatcher Matcher(EmailPattern, EMail);
	return Matcher.FindNext();
}

FString UXsollaLoginLibrary::GetStringCommandLineParam(const FString& ParamName)
{
	TCHAR Result[1024] = TEXT("");
	const bool FoundValue = FParse::Value(FCommandLine::Get(), *(ParamName + TEXT("=")), Result, UE_ARRAY_COUNT(Result));
	return FoundValue ? FString(Result) : FString(TEXT(""));
}

FString UXsollaLoginLibrary::GetSessionTicket()
{
	IOnlineSubsystem* OnlineInterface;
	OnlineInterface = IOnlineSubsystem::Get();
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

	bool isBGRA = Texture->PlatformData->PixelFormat == PF_B8G8R8A8;

	const FColor* FormatedImageData = reinterpret_cast<const FColor*>(Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));

	TArray<FColor> colorArray;
	colorArray.SetNumZeroed(Width * Height);

	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			colorArray[Y * Width + X] = FormatedImageData[Y * Width + X];
		}
	}

	Texture->PlatformData->Mips[0].BulkData.Unlock();

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
