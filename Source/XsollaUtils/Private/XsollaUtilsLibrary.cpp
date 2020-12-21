// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaUtilsLibrary.h"

#include "XsollaUtilsModule.h"

UXsollaUtilsLibrary::UXsollaUtilsLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaUtilsImageLoader* UXsollaUtilsLibrary::GetImageLoader()
{
	return FXsollaUtilsModule::Get().GetImageLoader();
}

FDateTime UXsollaUtilsLibrary::MakeDateTimeFromTimestamp(const int64 Time)
{
	return FDateTime::FromUnixTimestamp(Time);
}

int64 UXsollaUtilsLibrary::GetSecondsFromUnixTimestamp(const FDateTime DateTime)
{
	return DateTime.ToUnixTimestamp();
}