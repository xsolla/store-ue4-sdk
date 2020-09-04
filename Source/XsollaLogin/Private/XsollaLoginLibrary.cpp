// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginLibrary.h"

#include "XsollaLogin.h"

#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "Internationalization/Regex.h"
#include "Misc/Base64.h"
#include "Misc/CommandLine.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"

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

FString UXsollaLoginLibrary::ConvertTextureToString(UTexture2D* Texture)
{
	int w = Texture->GetSizeX();
	int h = Texture->GetSizeY();

	bool isBGRA = Texture->PlatformData->PixelFormat == PF_B8G8R8A8;

	const FColor* FormatedImageData = reinterpret_cast<const FColor*>(Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));

	TArray<FColor> colorArray;
	colorArray.SetNumZeroed(w * h);

	for (int32 X = 0; X < w; X++)
	{
		for (int32 Y = 0; Y < h; Y++)
		{
			colorArray[Y * w + X] = FormatedImageData[Y * w + X];
		}
	}

	Texture->PlatformData->Mips[0].BulkData.Unlock();

	EImageCompressionQuality LocalCompressionQuality = EImageCompressionQuality::Uncompressed;

	IImageWrapperModule* ImageWrapperModule = FModuleManager::LoadModulePtr<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWriter = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);

	const size_t BitsPerPixel = (sizeof(FColor) / 4) * 8;
	const ERGBFormat SourceChannelLayout = isBGRA ? ERGBFormat::BGRA : ERGBFormat::RGBA;

	ImageWriter->SetRaw((void*)&colorArray[0], sizeof(FColor) * w * h, w, h, SourceChannelLayout, BitsPerPixel);

	const TArray64<uint8>& CompressedData = ImageWriter->GetCompressed((int32)LocalCompressionQuality);

	TArray<uint8> data;
	for (int i = 0; i < CompressedData.Num(); ++i)
	{
		data.Add(CompressedData[i]);
	}

	FString encodedImageData = FBase64::Encode(data);

	return FString::Printf(TEXT("%s"), *encodedImageData);
}
