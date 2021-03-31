// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaJavaConvertor.h"

namespace XsollaJavaConvertor
{
	jstring GetJavaString(const FString& string)
	{
		JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
		jstring local = JEnv->NewStringUTF(TCHAR_TO_UTF8(*string));
		jstring result = (jstring)JEnv->NewGlobalRef(local);
		JEnv->DeleteLocalRef(local);
		return result;
	}

	FString FromJavaString(jstring javaString)
	{
		if (javaString == nullptr)
			return FString();

		JNIEnv* Env = AndroidJavaEnv::GetJavaEnv();
		const char* UTFString = Env->GetStringUTFChars(javaString, nullptr);
		FString Result(UTF8_TO_TCHAR(UTFString));
		Env->ReleaseStringUTFChars(javaString, UTFString);
		Env->DeleteLocalRef(javaString);

		return Result;
	}
}
