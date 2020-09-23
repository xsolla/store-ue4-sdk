// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXsollaLogin, Log, All);

#define VA_FUNC (FString(__FUNCTION__))				 // Current Class Name + Function Name where this is called
#define VA_LINE (FString::FromInt(__LINE__))		 // Current Line Number in the code where this is called
#define VA_FUNC_LINE (VA_FUNC + "(" + VA_LINE + ")") // Current Class and Line Number where this is called!

#define XSOLLA_LOGIN_VERSION TEXT("0.5.1")
