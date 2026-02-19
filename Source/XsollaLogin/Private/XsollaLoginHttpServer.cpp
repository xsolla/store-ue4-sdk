// Copyright 2024 Xsolla Inc. All Rights Reserved.

#include "XsollaLoginHttpServer.h"
#include "Common/TcpListener.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Async/Async.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "XsollaLoginDefines.h"

FXsollaLoginHttpServer::FXsollaLoginHttpServer()
	: Listener(nullptr)
	, Port(0)
{
}

FXsollaLoginHttpServer::~FXsollaLoginHttpServer()
{
	Stop();
}

static void ParseCsvLine(const FString& Line, TArray<FString>& OutCells)
{
	OutCells.Reset();

	FString Cell;
	bool bInQuotes = false;

	for (int32 i = 0; i < Line.Len(); ++i)
	{
		const TCHAR C = Line[i];

		if (C == '\"')
		{
			if (bInQuotes && i + 1 < Line.Len() && Line[i + 1] == '\"')
			{
				Cell.AppendChar('\"');
				++i;
			}
			else
			{
				bInQuotes = !bInQuotes;
			}
		}
		else if (C == ',' && !bInQuotes)
		{
			OutCells.Add(Cell);
			Cell.Reset();
		}
		else
		{
			Cell.AppendChar(C);
		}
	}

	OutCells.Add(Cell);

	for (FString& S : OutCells)
	{
		S.TrimStartAndEndInline();
		S.ReplaceInline(TEXT("\r"), TEXT(""));

		if (S.Len() >= 2 && S.StartsWith(TEXT("\"")) && S.EndsWith(TEXT("\"")))
		{
			S = S.Mid(1, S.Len() - 2);
		}
	}
}

bool FXsollaLoginHttpServer::Start(int32 InPort, const FOnAuthParamsReceived& InOnAuthParamsReceived)
{
	Stop();

	OnAuthParamsReceivedDelegate = InOnAuthParamsReceived;
	Port = InPort;

	// Cache culture info on the game thread (Start is called from GameThread)
	CachedCultureName = FInternationalization::Get().GetCurrentCulture().Get().GetName();
	CachedCultureName.ReplaceInline(TEXT("-"), TEXT("_"));
	CachedCultureName = CachedCultureName.ToLower();

	CachedTwoLetterCode = FInternationalization::Get().GetCurrentCulture().Get().GetTwoLetterISOLanguageName();
	CachedTwoLetterCode = CachedTwoLetterCode.ToLower();

	CachedSystemLocale = FPlatformMisc::GetDefaultLocale();
	CachedSystemLocale.ReplaceInline(TEXT("-"), TEXT("_"));
	CachedSystemLocale = CachedSystemLocale.ToLower();
	
	// Load localization and cache strings on Start (Game Thread)
	// This avoids doing file I/O and unsafe FInternationalization calls on the listener thread
	FString CsvContent;
	FString PluginContentDir = IPluginManager::Get().FindPlugin(TEXT("Xsolla"))->GetContentDir();
	FString CsvPath = PluginContentDir / TEXT("Resources/Login/XsollaLocalAuthLocalization.csv");
	
	// Default English keys
	FString SuccessTitleKey = TEXT("Successful login");
	FString SuccessMessageKey = TEXT("You can close this tab and return to the game");
	FString ErrorTitleKey = TEXT("Unsuccessful login");
	FString ErrorMessageKey = TEXT("Close this tab and try to log in again");

	// Default to English
	CachedSuccessTitle = SuccessTitleKey;
	CachedSuccessMessage = SuccessMessageKey;
	CachedErrorTitle = ErrorTitleKey;
	CachedErrorMessage = ErrorMessageKey;

	if (FFileHelper::LoadFileToString(CsvContent, *CsvPath))
	{
		UE_LOG(LogXsollaLogin, Log, TEXT("%s: Loaded localization file from path: %s"), *VA_FUNC_LINE, *CsvPath);
		
		TArray<FString> Lines;
		CsvContent.ParseIntoArray(Lines, TEXT("\n"), true);
		
		int32 LangIndex = 0;
		if (Lines.Num() > 0)
		{
			TArray<FString> Headers;
			ParseCsvLine(Lines[0], Headers);
			for (FString& H : Headers)
			{
				H.ReplaceInline(TEXT("\uFEFF"), TEXT("")); // BOM
				H.ReplaceInline(TEXT("\r"), TEXT(""));
				H.TrimStartAndEndInline();
				H = H.ToLower();
			}

			// Prioritize System Locale
			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Using cached System Locale: %s"), *VA_FUNC_LINE, *CachedSystemLocale);

			// 1. Try exact match with cached System Locale
			for (int32 i = 0; i < Headers.Num(); i++)
			{
				if (Headers[i] == CachedSystemLocale)
				{
					LangIndex = i;
					break;
				}
			}
			
			// 2. Try 2-letter match with cached System Locale
			if (LangIndex == 0)
			{
				FString SystemTwoLetter = CachedSystemLocale.Left(2);
				for (int32 i = 0; i < Headers.Num(); i++)
				{
					if (Headers[i] == SystemTwoLetter)
					{
						LangIndex = i;
						break;
					}
				}
			}

			// 2.5. Try matching language by 2-letter prefix (e.g. "es" -> "es_es")
			if (LangIndex == 0)
			{
				FString SystemTwoLetter = CachedSystemLocale.Left(2);
				for (int32 i = 0; i < Headers.Num(); i++)
				{
					if (Headers[i].StartsWith(SystemTwoLetter + TEXT("_")))
					{
						LangIndex = i;
						break;
					}
				}
			}

			// 3. Fallback to Engine Culture (exact match)
			if (LangIndex == 0)
			{
				for (int32 i = 0; i < Headers.Num(); i++)
				{
					if (Headers[i] == CachedCultureName)
					{
						LangIndex = i;
						break;
					}
				}
			}

			// 4. Fallback to Engine Culture (2-letter)
			if (LangIndex == 0)
			{
				for (int32 i = 0; i < Headers.Num(); i++)
				{
					if (Headers[i] == CachedTwoLetterCode)
					{
						LangIndex = i;
						break;
					}
				}
			}
			
			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Selected LangIndex: %d"), *VA_FUNC_LINE, LangIndex);
		}

		for (int32 i = 1; i < Lines.Num(); i++)
		{
			TArray<FString> Columns;
			ParseCsvLine(Lines[i], Columns);

			if (Columns.Num() > LangIndex)
			{
				FString Key = Columns[0];
				Key.ReplaceInline(TEXT("\uFEFF"), TEXT(""));
				Key.ReplaceInline(TEXT("\r"), TEXT(""));
				Key.TrimStartAndEndInline();
				FString LocalizedValue = Columns[LangIndex];

				if (Key == SuccessTitleKey)
				{
					CachedSuccessTitle = LocalizedValue;
				}
				else if (Key == SuccessMessageKey)
				{
					CachedSuccessMessage = LocalizedValue;
				}
				else if (Key == ErrorTitleKey)
				{
					CachedErrorTitle = LocalizedValue;
				}
				else if (Key == ErrorMessageKey)
				{
					CachedErrorMessage = LocalizedValue;
				}
			}
		}
	}
	else
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Failed to load localization file at path: %s"), *VA_FUNC_LINE, *CsvPath);
	}

	FIPv4Endpoint Endpoint(FIPv4Address::InternalLoopback, Port);
	Listener = new FTcpListener(Endpoint);

	if (!Listener)
	{
		return false;
	}

	Listener->OnConnectionAccepted().BindRaw(this, &FXsollaLoginHttpServer::HandleConnectionAccepted);

	return Listener->IsActive();
}

void FXsollaLoginHttpServer::Stop()
{
	if (Listener)
	{
		Listener->OnConnectionAccepted().Unbind();
		Listener->Stop();
		delete Listener;
		Listener = nullptr;
	}
}

int32 FXsollaLoginHttpServer::GetPort() const
{
	return Port;
}

bool FXsollaLoginHttpServer::HandleConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint)
{
	if (!ClientSocket)
	{
		return false;
	}

	TArray<uint8> ReceivedData;
	uint32 PendingDataSize = 0;

	// Simple read with timeout/retries could be better, but for now we assume request comes quickly
	int32 RetryCount = 0;
	while (RetryCount < 10)
	{
		if (ClientSocket->HasPendingData(PendingDataSize) && PendingDataSize > 0)
		{
			ReceivedData.SetNumUninitialized(PendingDataSize);
			int32 BytesRead = 0;
			if (ClientSocket->Recv(ReceivedData.GetData(), PendingDataSize, BytesRead))
			{
				ReceivedData.SetNum(BytesRead);
				break;
			}
		}
		FPlatformProcess::Sleep(0.01f);
		RetryCount++;
	}

	if (ReceivedData.Num() == 0)
	{
		ClientSocket->Close();
		return false;
	}

	ReceivedData.Add(0); // Null terminator
	FString RequestString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));

	bool bHasError = false;

	// Parse Request Line: GET /?code=... HTTP/1.1
	FString RequestLine;
	FString Leftover;
	if (RequestString.Split(TEXT("\r\n"), &RequestLine, &Leftover))
	{
		TArray<FString> Parts;
		RequestLine.ParseIntoArray(Parts, TEXT(" "), true);

		if (Parts.Num() >= 2 && Parts[0] == TEXT("GET"))
		{
			FString Url = Parts[1];
			FString QueryParams;
			FString UrlPath;

			if (Url.Split(TEXT("?"), &UrlPath, &QueryParams))
			{
				TMap<FString, FString> Params;
				TArray<FString> ParamPairs;
				QueryParams.ParseIntoArray(ParamPairs, TEXT("&"), true);

				for (const FString& Pair : ParamPairs)
				{
					FString Key, Value;
					if (Pair.Split(TEXT("="), &Key, &Value))
					{
						FString DecodedKey = FGenericPlatformHttp::UrlDecode(Key);
						FString DecodedValue = FGenericPlatformHttp::UrlDecode(Value);
						Params.Add(DecodedKey, DecodedValue);

						if (DecodedKey == TEXT("error") || DecodedKey == TEXT("error_code"))
						{
							bHasError = true;
						}
					}
					else
					{
						FString DecodedKey = FGenericPlatformHttp::UrlDecode(Pair);
						Params.Add(DecodedKey, TEXT(""));

						if (DecodedKey == TEXT("error") || DecodedKey == TEXT("error_code"))
						{
							bHasError = true;
						}
					}
				}

				// Execute on Game Thread
				AsyncTask(ENamedThreads::GameThread, [this, Params]()
				{
					OnAuthParamsReceivedDelegate.ExecuteIfBound(Params);
				});
			}
		}
	}

	// Send Response
	FString HtmlContent;

	// Use cached localized strings
	FString Title = bHasError ? CachedErrorTitle : CachedSuccessTitle;
	FString Message = bHasError ? CachedErrorMessage : CachedSuccessMessage;

	HtmlContent = FString::Printf(TEXT("<html><head><title>%s</title><meta charset=\"utf-8\"></head><body><h1>%s</h1><p>%s</p></body></html>"), *Title, *Title, *Message);
	SendResponse(ClientSocket, HtmlContent);

	ClientSocket->Close();
	return true;
}

void FXsollaLoginHttpServer::SendResponse(FSocket* ClientSocket, const FString& Content)
{
	// Calculate Content-Length using UTF-8 bytes length, not string length (which is number of TCHARs)
	FTCHARToUTF8 Utf8Content(*Content);
	FString Header = FString::Printf(TEXT("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\nConnection: close\r\n\r\n"), Utf8Content.Length());

	FString FullResponse = Header + Content;
	FTCHARToUTF8 Utf8Response(*FullResponse);

	int32 BytesSent = 0;
	ClientSocket->Send((const uint8*)Utf8Response.Get(), Utf8Response.Length(), BytesSent);
}

