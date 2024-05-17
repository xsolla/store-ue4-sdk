// Copyright 2024 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaCentrifugoDataModel.generated.h"

USTRUCT()
struct XSOLLASTORE_API FConnectionData
{
	GENERATED_BODY()

	UPROPERTY()
	FString auth;

	UPROPERTY()
	int32 project_id = 0;

	FConnectionData(const FString& Auth, int32 ProjectId)
		: auth(Auth)
		, project_id(ProjectId){};

	FConnectionData(){};
};

USTRUCT()
struct XSOLLASTORE_API FConnectCommand
{
	GENERATED_BODY()

	UPROPERTY()
	FConnectionData data;

	FConnectCommand(const FString& Auth, int32 ProjectId)
		: data(FConnectionData(Auth, ProjectId)){};

	FConnectCommand(){};
};

USTRUCT()
struct XSOLLASTORE_API FConnectionMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FConnectCommand connect;

	UPROPERTY()
	int32 id = 0;

	FConnectionMessage(const FString& Auth, int32 ProjectId, int32 Id)
		: connect(FConnectCommand(Auth, ProjectId))
		, id(Id){};

	FConnectionMessage(){};
};

USTRUCT()
struct XSOLLASTORE_API FOrderStatusData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 order_id = 0;

	UPROPERTY()
	FString status;
};

USTRUCT()
struct XSOLLASTORE_API FOrderStatusPub
{
	GENERATED_BODY()

	UPROPERTY()
	FOrderStatusData data;

	UPROPERTY()
	int32 offset = 0;
};

USTRUCT()
struct XSOLLASTORE_API FOrderStatusPush
{
	GENERATED_BODY()

	UPROPERTY()
	FOrderStatusPub pub;

	UPROPERTY()
	FString channel;
};

USTRUCT()
struct XSOLLASTORE_API FOrderStatusMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FOrderStatusPush push;
};