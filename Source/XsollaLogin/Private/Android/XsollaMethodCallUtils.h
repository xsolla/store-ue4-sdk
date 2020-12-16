// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

namespace XsollaMethodCallUtils
{
	void CallStaticVoidMethod(const ANSICHAR* ClassName, const ANSICHAR* MethodName, const ANSICHAR* MethodSignature, ...);
}
