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

bool FXsollaLoginHttpServer::Start(int32 InPort, const FOnAuthParamsReceived& InOnAuthParamsReceived)
{
	Stop();

	OnAuthParamsReceivedDelegate = InOnAuthParamsReceived;
	Port = InPort;

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
						Params.Add(FGenericPlatformHttp::UrlDecode(Key), FGenericPlatformHttp::UrlDecode(Value));
					}
					else
					{
						Params.Add(FGenericPlatformHttp::UrlDecode(Pair), TEXT(""));
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

	// Load localization
	FString CsvContent;
	FString CsvPath = FPaths::ProjectContentDir() / TEXT("Resources/Login/XsollaLocalAuthLocalization.csv");

	FString Title = TEXT("Successful login");
	FString Message = TEXT("You can close this tab and return to the game");

	if (FFileHelper::LoadFileToString(CsvContent, *CsvPath))
	{
		// Simple CSV parser
		TArray<FString> Lines;
		CsvContent.ParseIntoArray(Lines, TEXT("\n"), true);

		// Find language index
		int32 LangIndex = 0;
		if (Lines.Num() > 0)
		{
			TArray<FString> Headers;
			Lines[0].ParseIntoArray(Headers, TEXT(","), true);

			FString CultureName = FInternationalization::Get().GetCurrentCulture().Get().GetName();

			// Try exact match
			for (int32 i = 0; i < Headers.Num(); i++)
			{
				if (Headers[i].TrimStartAndEnd() == CultureName)
				{
					LangIndex = i;
					break;
				}
			}

			// Try 2-letter match if exact match failed
			if (LangIndex == 0)
			{
				FString TwoLetterCode = FInternationalization::Get().GetCurrentCulture().Get().GetTwoLetterISOLanguageName();
				for (int32 i = 0; i < Headers.Num(); i++)
				{
					if (Headers[i].TrimStartAndEnd() == TwoLetterCode)
					{
						LangIndex = i;
						break;
					}
				}
			}
		}

		for (int32 i = 1; i < Lines.Num(); i++)
		{
			// Manual parsing to handle quoted strings (basic implementation)
			// For this specific file structure we can just split by comma,
			// assuming no commas in unquoted strings or simple structure
			// But for safety let's use standard ParseIntoArray which handles basic CSV

			// Note: The CSV file provided has "Successful login" as the first column (key) implicitly
			// The header row is: en,ar,bg...
			// The data rows are: Successful login,translation1,translation2...

			TArray<FString> Columns;
			Lines[i].ParseIntoArray(Columns, TEXT(","), false); // Keep empty strings to maintain index

			// If split results in fewer columns than header, we might have issues, but let's try
			// Column 0 is English/Key

			if (Columns.Num() > LangIndex)
			{
				FString Key = Columns[0].TrimStartAndEnd();
				// Use English text as key since the file structure changed
				if (Key == TEXT("Successful login"))
				{
					Title = Columns[LangIndex].TrimStartAndEnd();
					Title.RemoveFromStart(TEXT("\""));
					Title.RemoveFromEnd(TEXT("\""));
				}
				else if (Key == TEXT("You can close this tab and return to the game"))
				{
					Message = Columns[LangIndex].TrimStartAndEnd();
					Message.RemoveFromStart(TEXT("\""));
					Message.RemoveFromEnd(TEXT("\""));
				}
			}
		}
	}

	HtmlContent = FString::Printf(TEXT("<html><head><title>%s</title><meta charset=\"utf-8\"></head><body><h1>%s</h1><p>%s</p></body></html>"), *Title, *Title, *Message);
	SendResponse(ClientSocket, HtmlContent);

	ClientSocket->Close();
	return true;
}

void FXsollaLoginHttpServer::SendResponse(FSocket* ClientSocket, const FString& Content)
{
	FString Header = FString::Printf(TEXT("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\nConnection: close\r\n\r\n"), Content.Len());

	FString FullResponse = Header + Content;
	FTCHARToUTF8 Utf8Response(*FullResponse);

	int32 BytesSent = 0;
	ClientSocket->Send((const uint8*)Utf8Response.Get(), Utf8Response.Length(), BytesSent);
}

