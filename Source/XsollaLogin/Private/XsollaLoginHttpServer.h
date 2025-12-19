// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

class FTcpListener;
class FSocket;

typedef TMap<FString, FString> FAuthParamsMap;
DECLARE_DELEGATE_OneParam(FOnAuthParamsReceived, const FAuthParamsMap&);

class FXsollaLoginHttpServer
{
public:
	FXsollaLoginHttpServer();
	~FXsollaLoginHttpServer();

	bool Start(int32 Port, const FOnAuthParamsReceived& OnAuthParamsReceived);
	void Stop();
	int32 GetPort() const;

private:
	bool HandleConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint);
	void SendResponse(FSocket* ClientSocket, const FString& Content);

	FTcpListener* Listener;
	FOnAuthParamsReceived OnAuthParamsReceivedDelegate;
	int32 Port;
};



