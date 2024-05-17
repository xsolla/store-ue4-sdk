// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"
#endif

namespace XsollaJavaConvertor
{
#if PLATFORM_ANDROID
	jstring GetJavaString(const FString& string);
	FString FromJavaString(jstring javaString);
#endif
};
