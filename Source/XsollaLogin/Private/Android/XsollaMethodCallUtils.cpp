// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaMethodCallUtils.h"

namespace XsollaMethodCallUtils
{
	void CallStaticVoidMethod(const ANSICHAR* ClassName, const ANSICHAR* MethodName, const ANSICHAR* MethodSignature, ...)
	{
		bool bIsOptional = false;

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		jclass Class = FAndroidApplication::FindJavaClass(ClassName);

		jmethodID Method = FJavaWrapper::FindStaticMethod(Env, Class, MethodName, MethodSignature, bIsOptional);

		va_list Args;
		va_start(Args, MethodSignature);
		Env->CallStaticVoidMethodV(Class, Method, Args);
		va_end(Args);

		Env->DeleteLocalRef(Class);
	}
}
