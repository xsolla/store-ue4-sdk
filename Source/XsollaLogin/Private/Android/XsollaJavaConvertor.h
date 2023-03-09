// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Android/AndroidApplication.h"
#include "Launch/Public/Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

namespace XsollaJavaConvertor
{
#if PLATFORM_ANDROID
	jstring GetJavaString(const FString& string);
	FString FromJavaString(jstring javaString);
#endif
};
