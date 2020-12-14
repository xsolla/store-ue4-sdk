// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSave.h"
#include "XsollaLogin.h"
#include "XsollaLoginSettings.h"
#include "XsollaLoginDefines.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/Base64.h"

const FString UXsollaLoginSave::SaveSlotName = "XsollaLoginSaveSlot";
const int32 UXsollaLoginSave::UserIndex = 0;

FXsollaLoginData UXsollaLoginSave::Load()
{
	if(!UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		return FXsollaLoginData();		
	}
	
	const UXsollaLoginSave* SaveInstance = Cast<UXsollaLoginSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if (!SaveInstance)
	{
		return FXsollaLoginData();
	}

	// Decrypt the players sensitive credentials using an AES-256 encryption key if enabled
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	FXsollaLoginData CachedLoginData = SaveInstance->LoginData;

	if (Settings->EncryptCachedCredentials && CachedLoginData.bEncrypted)
	{
		const FAES::FAESKey XsollaSaveEncryptionKey = GetEncryptionKey();

		if (XsollaSaveEncryptionKey.IsValid())
		{
			CachedLoginData.AuthToken.JWT = DecryptString(CachedLoginData.AuthToken.JWT, XsollaSaveEncryptionKey);
			CachedLoginData.Username = DecryptString(CachedLoginData.Username, XsollaSaveEncryptionKey);
			CachedLoginData.Password = DecryptString(CachedLoginData.Password, XsollaSaveEncryptionKey);
		}
		else
		{
			UE_LOG(LogXsollaLogin, Error, TEXT("Xsolla Login Save Encryption Key was Invalid!"));
		}
	}
	else
	{
		UE_LOG(LogXsollaLogin, Verbose, TEXT("Xsolla Login Save Encryption is disabled, skipping."));
	}

	return CachedLoginData;
}

void UXsollaLoginSave::Save(const FXsollaLoginData& InLoginData)
{
	UXsollaLoginSave* SaveInstance = Cast<UXsollaLoginSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if (!SaveInstance)
	{
		SaveInstance = Cast<UXsollaLoginSave>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
	}

	// Encrypt the players sensitive credentials using a secondary AES-256 encryption key configured for the project if enabled
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	FXsollaLoginData CachedLoginData = InLoginData;

	if (Settings->EncryptCachedCredentials)
	{
		const FAES::FAESKey XsollaSaveEncryptionKey = GetEncryptionKey();

		if (XsollaSaveEncryptionKey.IsValid())
		{
			const FString KeyStr = FBase64::Encode(XsollaSaveEncryptionKey.Key, XsollaSaveEncryptionKey.KeySize);

			CachedLoginData.AuthToken.JWT = EncryptString(CachedLoginData.AuthToken.JWT, XsollaSaveEncryptionKey);
			CachedLoginData.Username = EncryptString(CachedLoginData.Username, XsollaSaveEncryptionKey);
			CachedLoginData.Password = EncryptString(CachedLoginData.Password, XsollaSaveEncryptionKey);
			CachedLoginData.bEncrypted = true;
		}
		else
		{
			UE_LOG(LogXsollaLogin, Error, TEXT("Xsolla Login Save Encryption Key was Invalid!"));
		}
	}
	else
	{
		UE_LOG(LogXsollaLogin, Verbose, TEXT("Xsolla Login Save Encryption is disabled, skipping."));
	}

	SaveInstance->LoginData = CachedLoginData;

	UGameplayStatics::SaveGameToSlot(SaveInstance, SaveSlotName, 0);
}

FAES::FAESKey UXsollaLoginSave::GetEncryptionKey()
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	FAES::FAESKey CachedEncryptionKey = FAES::FAESKey();

	if (Settings->XsollaSaveEncryptionKey.IsEmpty())
	{
		return CachedEncryptionKey;
	}

	TArray<uint8> Key;
	if (FBase64::Decode(Settings->XsollaSaveEncryptionKey, Key))
	{
		check(Key.Num() == sizeof(FAES::FAESKey::Key));

		FMemory::Memcpy(CachedEncryptionKey.Key, &Key[0], sizeof(FAES::FAESKey::Key));

		check(CachedEncryptionKey.IsValid());
	}

	return CachedEncryptionKey;
}

FString UXsollaLoginSave::EncryptString(const FString& InString, const FAES::FAESKey& InKey)
{
	if (InString.IsEmpty())
	{
		return FString();
	}

	if (!InKey.IsValid())
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("EncryptString received invalid AES-256 key!"));

		return FString();
	}

	// For simplicities sake, trim away aes block padding by splitting left of the || symbol
	const FString PadStr = FString::Printf(TEXT("%s||"), *InString);

	TArray<uint8> Bytes;
	const uint32 NumBytes = Align(PadStr.Len(), FAES::AESBlockSize);

	Bytes.SetNumUninitialized(NumBytes);

	StringToBytes(PadStr, Bytes.GetData(), Bytes.Num());

	FAES::EncryptData(Bytes.GetData(), NumBytes, InKey);

	return BytesToString(Bytes.GetData(), NumBytes);
}

FString UXsollaLoginSave::DecryptString(const FString& InString, const FAES::FAESKey& InKey)
{
	if (InString.IsEmpty() || !InKey.IsValid())
	{
		return FString();
	}

	if (!InKey.IsValid())
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("DecryptString received invalid AES-256 key!"));

		return FString();
	}

	TArray<uint8> Bytes;
	const uint32 NumBytes = Align(InString.Len(), FAES::AESBlockSize);

	Bytes.SetNumUninitialized(NumBytes);

	StringToBytes(InString, Bytes.GetData(), Bytes.Num());

	FAES::DecryptData(Bytes.GetData(), NumBytes, InKey);

	const FString DecryptedString = BytesToString(Bytes.GetData(), NumBytes);
	FString LeftString;
	FString RightString;

	DecryptedString.Split("||", &LeftString, &RightString, ESearchCase::CaseSensitive);

	return LeftString;
}
