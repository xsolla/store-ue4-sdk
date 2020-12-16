// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

namespace XsollaJavaConvertor
{
	jstring GetJavaString(const FString& string);
	FString FromJavaString(jstring javaString);
};
