// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

namespace XsollaMethodCallUtils
{
#if PLATFORM_ANDROID
	void CallStaticVoidMethod(const ANSICHAR* ClassName, const ANSICHAR* MethodName, const ANSICHAR* MethodSignature, ...);
#endif
}
