// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginController.h"

#include "XsollaLogin.h"
#include "XsollaLoginDefines.h"
#include "XsollaLoginSave.h"
#include "XsollaLoginSettings.h"

#include "Json.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Launch/Resources/Version.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

const FString UXsollaLoginController::RegistrationEndpoint(TEXT("https://login.xsolla.com/api/user"));
const FString UXsollaLoginController::LoginEndpoint(TEXT("https://login.xsolla.com/api/login"));
const FString UXsollaLoginController::ResetPasswordEndpoint(TEXT("https://login.xsolla.com/api/password/reset/request"));

const FString UXsollaLoginController::ProxyRegistrationEndpoint(TEXT("https://login.xsolla.com/api/proxy/registration"));
const FString UXsollaLoginController::ProxyLoginEndpoint(TEXT("https://login.xsolla.com/api/proxy/login"));
const FString UXsollaLoginController::ProxyResetPasswordEndpoint(TEXT("https://login.xsolla.com/api/proxy/password/reset"));

UXsollaLoginController::UXsollaLoginController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXsollaLoginController::Initialize(const FString& InLoginProjectId)
{
	LoginProjectId = InLoginProjectId;
}

void UXsollaLoginController::RegistrateUser(const FString& Username, const FString& Password, const FString& Email, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetStringField(TEXT("email"), Email);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? RegistrationEndpoint : ProxyRegistrationEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginController::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginController::AuthenticateUser(const FString& Username, const FString& Password, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, bool bRememberMe)
{
	// Be sure we've dropped any saved info
	LoginData = FXsollaLoginData();
	LoginData.Username = Username;
	LoginData.Password = Password;
	LoginData.bRememberMe = bRememberMe;
	SaveData();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetBoolField(TEXT("remember_me"), false);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? LoginEndpoint : ProxyLoginEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginController::UserLogin_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginController::ResetUserPassword(const FString& Username, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("username"), Username);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? ResetPasswordEndpoint : ProxyResetPasswordEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginController::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginController::ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	RequestDataJson->SetStringField(TEXT("token"), LoginData.AuthToken.JWT);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = Settings->VerifyTokenURL;

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginController::TokenVerify_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginController::Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaLoginController::UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString LoginUrlFieldName = TEXT("login_url");
		if (JsonObject->HasTypedField<EJson::String>(LoginUrlFieldName))
		{
			FString LoginUrl = JsonObject.Get()->GetStringField(LoginUrlFieldName);
			FString UrlOptions = LoginUrl.RightChop(LoginUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));

			LoginData.AuthToken.JWT = UGameplayStatics::ParseOption(UrlOptions, TEXT("token"));
			LoginData.bRememberMe = UGameplayStatics::ParseOption(UrlOptions, TEXT("remember_me")).ToBool();
			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);

			// Check if verification URL is provided
			if (FXsollaLoginModule::Get().GetSettings()->VerifyTokenURL.IsEmpty())
			{
				UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: No VerifyTokenURL is set, skip token verification step"), *VA_FUNC_LINE);

				SuccessCallback.ExecuteIfBound(LoginData);
			}
			else
			{
				// Start verification process now
				ValidateToken(SuccessCallback, ErrorCallback);
			}

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *LoginUrlFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginController::TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	// If no error happend so token is verified now
	LoginData.AuthToken.bIsVerified = true;
	SaveData();

	SuccessCallback.ExecuteIfBound(LoginData);
}

bool UXsollaLoginController::HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError ErrorCallback)
{
	FString ErrorStr;
	FString ErrorCode = TEXT("204");
	FString ResponseStr = TEXT("invalid");

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();

		if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			ErrorCode = FString::Printf(TEXT("%d"), HttpResponse->GetResponseCode());
			ErrorStr = FString::Printf(TEXT("Invalid response. code=%d error=%s"), HttpResponse->GetResponseCode(), *ResponseStr);

			// Example: {"error":{"code":"003-003","description":"The username is already taken"}}
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				static const FString ErrorFieldName = TEXT("error");
				if (JsonObject->HasTypedField<EJson::Object>(ErrorFieldName))
				{
					TSharedPtr<FJsonObject> ErrorObject = JsonObject.Get()->GetObjectField(ErrorFieldName);
					ErrorCode = ErrorObject.Get()->GetStringField(TEXT("code"));
					ErrorStr = ErrorObject.Get()->GetStringField(TEXT("description"));
				}
				else
				{
					ErrorStr = FString::Printf(TEXT("Can't deserialize error json: no field '%s' found"), *ErrorFieldName);
				}
			}
			else
			{
				ErrorStr = TEXT("Can't deserialize error json");
			}
		}
	}
	else
	{
		ErrorStr = TEXT("No response");
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: request failed (%s): %s"), *VA_FUNC_LINE, *ErrorStr, *ResponseStr);
		ErrorCallback.ExecuteIfBound(ErrorCode, ErrorStr);
		return true;
	}

	return false;
}

TSharedRef<IHttpRequest> UXsollaLoginController::CreateHttpRequest(const FString& Url, const FString& Content)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	// Temporal solution with headers processing on server-side #37
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=login&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_LOGIN_VERSION);

	HttpRequest->SetURL(Url + MetaUrl);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetContentAsString(Content);

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("engine"), TEXT("ue4"));
	HttpRequest->SetHeader(TEXT("engine_v"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("sdk"), TEXT("login"));
	HttpRequest->SetHeader(TEXT("sdk_v"), XSOLLA_LOGIN_VERSION);

	return HttpRequest;
}

FXsollaLoginData UXsollaLoginController::GetLoginData()
{
	return LoginData;
}

void UXsollaLoginController::DropLoginData()
{
	LoginData = FXsollaLoginData();

	// Drop saved data too
	UXsollaLoginSave::Save(LoginData);
}

void UXsollaLoginController::LoadSavedData()
{
	LoginData = UXsollaLoginSave::Load();
}

void UXsollaLoginController::SaveData()
{
	if (LoginData.bRememberMe)
	{
		UXsollaLoginSave::Save(LoginData);
	}
	else
	{
		// Dron't drop cache in memory but reset save file
		UXsollaLoginSave::Save(FXsollaLoginData());
	}
}

#undef LOCTEXT_NAMESPACE
