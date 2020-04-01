// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginLibrary.h"

#include "XsollaLogin.h"

#include "Engine/Engine.h"
#include "Internationalization/Regex.h"
#include "Misc/CommandLine.h"
#include "Online.h"
#include "OnlineSubsystem.h"

UXsollaLoginLibrary::UXsollaLoginLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UXsollaLoginSettings* UXsollaLoginLibrary::GetLoginSettings()
{
	return FXsollaLoginModule::Get().GetSettings();
}

bool UXsollaLoginLibrary::IsEmailValid(const FString& EMail)
{
	FRegexPattern EmailPattern(TEXT("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"));
	FRegexMatcher Matcher(EmailPattern, EMail);
	return Matcher.FindNext();
}

FString UXsollaLoginLibrary::GetStringCommandLineParam(const FString& ParamName)
{
	TCHAR Result[1024] = TEXT("");
	const bool FoundValue = FParse::Value(FCommandLine::Get(), *(ParamName + TEXT("=")), Result, UE_ARRAY_COUNT(Result));
	return FoundValue ? FString(Result) : FString(TEXT(""));
}

FString UXsollaLoginLibrary::GetSessionTicket()
{
	IOnlineSubsystem* OnlineInterface;
	OnlineInterface = IOnlineSubsystem::Get();
	FString SessionTicket = OnlineInterface->GetIdentityInterface()->GetAuthToken(0);
	return SessionTicket;
}
