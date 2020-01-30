// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaStoreImageLoader.h"

#include "XsollaStoreDefines.h"

#include "Framework/Application/SlateApplication.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/SecureHash.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

UXsollaStoreImageLoader::UXsollaStoreImageLoader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXsollaStoreImageLoader::LoadImage(FString URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	UE_LOG(LogXsollaStore, VeryVerbose, TEXT("%s: Loading image from: %s"), *VA_FUNC_LINE, *URL);

	const FString ResourceId = GetCacheName(URL).ToString();
	if (ImageBrushes.Contains(ResourceId))
	{
		UE_LOG(LogXsollaStore, VeryVerbose, TEXT("%s: Loaded from cache: %s"), *VA_FUNC_LINE, *ResourceId);
		SuccessCallback.ExecuteIfBound(*ImageBrushes.Find(ResourceId)->Get());
	}
	else
	{
		if (PendingRequests.Contains(ResourceId))
		{
			PendingRequests[ResourceId].AddLambda([=](bool IsCompleted) {
				if (IsCompleted)
				{
					UE_LOG(LogXsollaStore, VeryVerbose, TEXT("%s: Loaded from cache: %s"), *VA_FUNC_LINE, *ResourceId);
					SuccessCallback.ExecuteIfBound(*ImageBrushes.Find(ResourceId)->Get());
				}
				else
				{
					UE_LOG(LogXsollaStore, Error, TEXT("%s: Failed to get image"), *VA_FUNC_LINE);
					ErrorCallback.ExecuteIfBound();
				}
			});
		}
		else
		{
			FOnRequestCompleted imageLoadingCompletedDelegate;
			PendingRequests.Add(ResourceId, imageLoadingCompletedDelegate);

			TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

			HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaStoreImageLoader::LoadImage_HttpRequestComplete, SuccessCallback, ErrorCallback);
			HttpRequest->SetURL(URL);
			HttpRequest->SetVerb(TEXT("GET"));

			HttpRequest->ProcessRequest();
		}
	}
}

void UXsollaStoreImageLoader::LoadImage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnImageLoaded SuccessCallback, FOnImageLoadFailed ErrorCallback)
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
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Invalid image wrapper"), *VA_FUNC_LINE);
		}
		else if (ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
		{
			const int32 BytesPerPixel = ImageWrapper->GetBitDepth();
			const TArray<uint8>* RawData = nullptr;

			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, BytesPerPixel, RawData) && RawData && RawData->Num() > 0)
			{
				if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(ResourceName, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), *RawData))
				{
					TSharedPtr<FSlateDynamicImageBrush> ImageBrush = MakeShareable(new FSlateDynamicImageBrush(ResourceName, FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
					ImageBrushes.Add(ResourceName.ToString(), ImageBrush);

					SuccessCallback.ExecuteIfBound(*ImageBrush.Get());

					PendingRequests[ResourceName.ToString()].Broadcast(true);
					PendingRequests.Remove(ResourceName.ToString());

					return;
				}
				else
				{
					UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't generate resource"), *VA_FUNC_LINE);
				}
			}
			else
			{
				UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't get raw data"), *VA_FUNC_LINE);
			}
		}
		else
		{
			UE_LOG(LogXsollaStore, Error, TEXT("%s: Can't load compressed data"), *VA_FUNC_LINE);
		}
	}
	else
	{
		UE_LOG(LogXsollaStore, Error, TEXT("%s: Failed to download image"), *VA_FUNC_LINE);
	}

	ErrorCallback.ExecuteIfBound();

	if (PendingRequests.Contains(ResourceName.ToString()))
	{
		PendingRequests[ResourceName.ToString()].Broadcast(false);
		PendingRequests.Remove(ResourceName.ToString());
	}
}

FName UXsollaStoreImageLoader::GetCacheName(const FString& URL) const
{
	return FName(*FString::Printf(TEXT("XsollaStoreImage_%s"), *FMD5::HashAnsiString(*URL)));
}

#undef LOCTEXT_NAMESPACE
