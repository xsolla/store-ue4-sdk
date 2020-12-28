// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsImageLoader.h"

#include "Framework/Application/SlateApplication.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/SecureHash.h"
#include "Modules/ModuleManager.h"
#include "XsollaUtilsDefines.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

UXsollaUtilsImageLoader::UXsollaUtilsImageLoader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXsollaUtilsImageLoader::LoadImage(FString URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	if (URL.IsEmpty())
	{
		UE_LOG(LogXsollaUtils, Log, TEXT("%s: Found empty String URL"), *VA_FUNC_LINE);
		return;
	}
	UE_LOG(LogXsollaUtils, VeryVerbose, TEXT("%s: Loading image from: %s"), *VA_FUNC_LINE, *URL);

	const FString ResourceId = GetCacheName(URL).ToString();
	if (ImageBrushes.Contains(ResourceId))
	{
		UE_LOG(LogXsollaUtils, VeryVerbose, TEXT("%s: Loaded from cache: %s"), *VA_FUNC_LINE, *ResourceId);
		SuccessCallback.ExecuteIfBound(*ImageBrushes.Find(ResourceId)->Get(), URL);
	}
	else
	{
		if (PendingRequests.Contains(ResourceId))
		{
			PendingRequests[ResourceId].AddLambda([=](bool IsCompleted) {
				if (IsCompleted)
				{
					UE_LOG(LogXsollaUtils, VeryVerbose, TEXT("%s: Loaded from cache: %s"), *VA_FUNC_LINE, *ResourceId);
					SuccessCallback.ExecuteIfBound(*ImageBrushes.Find(ResourceId)->Get(), URL);
				}
				else
				{
					UE_LOG(LogXsollaUtils, Error, TEXT("%s: Failed to get image"), *VA_FUNC_LINE);
					ErrorCallback.ExecuteIfBound(URL);
				}
			});
		}
		else
		{
			FOnRequestCompleted imageLoadingCompletedDelegate;
			PendingRequests.Add(ResourceId, imageLoadingCompletedDelegate);

			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

			HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaUtilsImageLoader::LoadImage_HttpRequestComplete, SuccessCallback, ErrorCallback);
			HttpRequest->SetURL(URL);
			HttpRequest->SetVerb(TEXT("GET"));

			HttpRequest->ProcessRequest();
		}
	}
}

void UXsollaUtilsImageLoader::LoadImage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnImageLoaded SuccessCallback, FOnImageLoadFailed ErrorCallback)
{
	const FName ResourceName = GetCacheName(HttpRequest->GetURL());

	if (bSucceeded && HttpResponse.IsValid())
	{
		const TArray<uint8>& ImageData = HttpResponse->GetContent();

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		const EImageFormat ImageType = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageType);

		if (!ImageWrapper.IsValid())
		{
			UE_LOG(LogXsollaUtils, Error, TEXT("%s: Invalid image wrapper"), *VA_FUNC_LINE);
		}
		else if (ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
		{
			const int32 BytesPerPixel = ImageWrapper->GetBitDepth();
			TArray<uint8> RawData;

			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, BytesPerPixel, RawData) && RawData.Num() > 0)
			{
				if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(ResourceName, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), RawData))
				{
					TSharedPtr<FSlateDynamicImageBrush> ImageBrush = MakeShareable(new FSlateDynamicImageBrush(ResourceName, FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
					ImageBrushes.Add(ResourceName.ToString(), ImageBrush);

					SuccessCallback.ExecuteIfBound(*ImageBrush.Get(), HttpRequest->GetURL());

					PendingRequests[ResourceName.ToString()].Broadcast(true);
					PendingRequests.Remove(ResourceName.ToString());

					return;
				}
				else
				{
					UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't generate resource"), *VA_FUNC_LINE);
				}
			}
			else
			{
				UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't get raw data"), *VA_FUNC_LINE);
			}
		}
		else
		{
			UE_LOG(LogXsollaUtils, Error, TEXT("%s: Can't load compressed data"), *VA_FUNC_LINE);
		}
	}
	else
	{
		UE_LOG(LogXsollaUtils, Error, TEXT("%s: Failed to download image"), *VA_FUNC_LINE);
	}

	ErrorCallback.ExecuteIfBound(HttpRequest->GetURL());

	if (PendingRequests.Contains(ResourceName.ToString()))
	{
		PendingRequests[ResourceName.ToString()].Broadcast(false);
		PendingRequests.Remove(ResourceName.ToString());
	}
}

FName UXsollaUtilsImageLoader::GetCacheName(const FString& URL) const
{
	return FName(*FString::Printf(TEXT("XsollaUtilsImage_%s"), *FMD5::HashAnsiString(*URL)));
}

#undef LOCTEXT_NAMESPACE
