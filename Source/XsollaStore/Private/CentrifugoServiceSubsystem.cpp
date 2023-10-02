// Copyright 2023 Xsolla Inc. All Rights Reserved.

#include "CentrifugoServiceSubsystem.h"
#include "XsollaOrderCheckObject.h"
#include "CentrifugoClient.h"
#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"
#include "XsollaLoginSubsystem.h"
#include "XsollaStoreDefines.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "XsollaCentrifugoDataModel.h"
#include "Engine/GameInstance.h"

void UCentrifugoServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	LoginSubsystem = UGameInstance::GetSubsystem<UXsollaLoginSubsystem>(GetGameInstance());

	UE_LOG(LogXsollaCentrifugo, Log, TEXT("%s: CentrifugoService subsystem initialized"), *VA_FUNC_LINE);
}

void UCentrifugoServiceSubsystem::AddTracker(const UXsollaOrderCheckObject* Tracker)
{
	Trackers.Add(Tracker);

	if (CentrifugoClient == nullptr)
	{
		CreateCentrifugoClient();
	}
}

void UCentrifugoServiceSubsystem::RemoveTracker(const UXsollaOrderCheckObject* Tracker)
{
	Trackers.Remove(Tracker);

	if (Trackers.Num() == 0 && CentrifugoClient != nullptr)
	{
		TerminateCentrifugoClient();
	}
	
}

void UCentrifugoServiceSubsystem::CreateCentrifugoClient()
{
	CentrifugoClient = NewObject<UCentrifugoClient>();
	CentrifugoClient->MessageReceived.BindUObject(this, &UCentrifugoServiceSubsystem::OnCentrifugoMessageReceived);
	CentrifugoClient->Error.BindUObject(this, &UCentrifugoServiceSubsystem::OnCentrifugoError);
	CentrifugoClient->Closed.BindUObject(this, &UCentrifugoServiceSubsystem::OnCentrifugoClosed);
	CentrifugoClient->Connect();

	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	int32 Id = static_cast<int32>(GetTypeHash(FGuid::NewGuid().ToString()));
	Id = FMath::Abs(Id);
	int32 ProjectId = FCString::Atoi(*Settings->ProjectID);

	FConnectionMessage ConnectionMessage = FConnectionMessage(LoginSubsystem->GetLoginData().AuthToken.JWT, ProjectId, Id);
	TSharedRef<FJsonObject> ConnectionMessageJson = MakeShareable(new FJsonObject());
	if (FJsonObjectConverter::UStructToJsonObject(FConnectionMessage::StaticStruct(), &ConnectionMessage, ConnectionMessageJson, 0, 0))
	{
		FString Data;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Data);
		FJsonSerializer::Serialize(ConnectionMessageJson, Writer);
		CentrifugoClient->Send(Data);
	}

	GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, this, &UCentrifugoServiceSubsystem::DoPing, 1.f, true);
	UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo client created"));
}

void UCentrifugoServiceSubsystem::TerminateCentrifugoClient()
{
	CentrifugoClient->MessageReceived.Unbind();
	CentrifugoClient->Error.Unbind();
	CentrifugoClient->Closed.Unbind();
	CentrifugoClient->Disconnect();
	CentrifugoClient = nullptr;

	GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);

	UE_LOG(LogXsollaCentrifugo, Log, TEXT("Centrifugo client terminated"));
}

void UCentrifugoServiceSubsystem::OnCentrifugoMessageReceived(const FString& Message)
{
	PingCounter = 0;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Message);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		FOrderStatusMessage OrderStatusMessage;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FOrderStatusMessage::StaticStruct(), &OrderStatusMessage))
		{
			OrderStatusUpdated.Broadcast(OrderStatusMessage.push.pub.data);
			return;
		}

		static const FString ConnectFieldName = TEXT("connect");
		if (JsonObject->HasTypedField<EJson::Object>(ConnectFieldName))
		{
			UE_LOG(LogXsollaCentrifugo, Log, TEXT("Connect message received."));
			return;
		}

		UE_LOG(LogXsollaCentrifugo, Warning, TEXT("Can't parse received centrifugo message into OrderStatusMessage."));
	 }
	else
	{
		UE_LOG(LogXsollaCentrifugo, Warning, TEXT("Can't parse received centrifugo message."));
	}
}

void UCentrifugoServiceSubsystem::OnCentrifugoError(const FString& ErrorMessage)
{
	Error.Broadcast();
}

void UCentrifugoServiceSubsystem::OnCentrifugoClosed(const FString& Reason)
{
	Close.Broadcast();
}

void UCentrifugoServiceSubsystem::DoPing()
{
	PingCounter += 1;
	if (PingCounter >= PingInterval)
	{
		PingCounter = 0;
		CentrifugoClient->SendPing();
		if (CentrifugoClient->IsAlive())
		{
			TimeoutCounter = 0;
		}
		else
		{
			TimeoutCounter += PingInterval;
			if (TimeoutCounter >= TimeoutLimit)
			{
				UE_LOG(LogXsollaCentrifugo, Warning, TEXT("Centrifugo connection timeout limit exceeded"));
				OnCentrifugoClosed("Network problems");
				GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);
			}
		}
	}
}
