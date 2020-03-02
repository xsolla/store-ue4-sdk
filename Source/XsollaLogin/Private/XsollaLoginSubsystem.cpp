// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSubsystem.h"

#include "XsollaLoginDefines.h"
#include "XsollaLoginLibrary.h"
#include "XsollaLoginSave.h"
#include "XsollaLoginSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Dom/JsonObject.h"
#include "Engine/Engine.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Base64.h"
#include "OnlineSubsystem.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

const FString UXsollaLoginSubsystem::RegistrationEndpoint(TEXT("https://login.xsolla.com/api/user"));
const FString UXsollaLoginSubsystem::LoginEndpoint(TEXT("https://login.xsolla.com/api/login"));
const FString UXsollaLoginSubsystem::LoginSocialEndpoint(TEXT("https://login.xsolla.com/api/social"));
const FString UXsollaLoginSubsystem::ResetPasswordEndpoint(TEXT("https://login.xsolla.com/api/password/reset/request"));

const FString UXsollaLoginSubsystem::ProxyRegistrationEndpoint(TEXT("https://login.xsolla.com/api/proxy/registration"));
const FString UXsollaLoginSubsystem::ProxyLoginEndpoint(TEXT("https://login.xsolla.com/api/proxy/login"));
const FString UXsollaLoginSubsystem::ProxyResetPasswordEndpoint(TEXT("https://login.xsolla.com/api/proxy/password/reset"));

const FString UXsollaLoginSubsystem::ValidateTokenEndpoint(TEXT("https://login.xsolla.com/api/token/validate"));

const FString UXsollaLoginSubsystem::UserAttributesEndpoint(TEXT("https://login.xsolla.com/api/attributes"));

UXsollaLoginSubsystem::UXsollaLoginSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/Xsolla/Browser/W_LoginBrowser.W_LoginBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
}

void UXsollaLoginSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Settings = NewObject<UXsollaLoginSettings>(GetTransientPackage(), "XsollaLoginSettings", RF_Standalone);

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaLogin",
			LOCTEXT("RuntimeSettingsName", "Xsolla Login"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Login"),
			Settings);
	}

	Initialize(Settings->ProjectId, Settings->LoginProjectID);

	LoadSavedData();

	UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaLoginSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaLoginSubsystem::Initialize(const FString& InProjectId, const FString& InLoginProjectId)
{
	ProjectId = InProjectId;
	LoginProjectId = InLoginProjectId;

	// Check token override from Xsolla Launcher
	FString LauncherLoginJwt = UXsollaLoginLibrary::GetStringCommandLineParam(TEXT("xsolla-login-jwt"));
	if (!LauncherLoginJwt.IsEmpty())
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Xsolla Launcher login token is used"), *VA_FUNC_LINE);
		LoginData.AuthToken.JWT = LauncherLoginJwt;
	}
}

void UXsollaLoginSubsystem::RegistrateUser(const FString& Username, const FString& Password, const FString& Email, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User registration should be handled via Steam"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Registration failed"), TEXT("User registration should be handled via Steam"));
		return;
	}

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetStringField(TEXT("email"), Email);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? RegistrationEndpoint : ProxyRegistrationEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateUser(const FString& Username, const FString& Password, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, bool bRememberMe)
{
	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User authentication should be handled via Steam"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Authentication failed"), TEXT("User authentication should be handled via Steam"));
		return;
	}

	// Be sure we've dropped any saved info
	LoginData = FXsollaLoginData();
	LoginData.Username = Username;
	LoginData.Password = Password;
	LoginData.bRememberMe = bRememberMe;
	SaveData();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetBoolField(TEXT("remember_me"), false);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? LoginEndpoint : ProxyLoginEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserLogin_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ResetUserPassword(const FString& Username, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User password reset should be handled via Steam"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Password reset failed"), TEXT("User password reset should be handled via Steam"));
		return;
	}

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? ResetPasswordEndpoint : ProxyResetPasswordEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("token"), LoginData.AuthToken.JWT);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = (!Settings->VerifyTokenURL.IsEmpty()) ? Settings->VerifyTokenURL : ValidateTokenEndpoint;

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::TokenVerify_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetSocialAuthenticationUrl(const FString& ProviderName, const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/%s/login_url?projectId=%s&login_url=%s"),
		*LoginSocialEndpoint,
		*ProviderName,
		*LoginProjectId,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, TEXT(""));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::LaunchSocialAuthentication(const FString& SocialAuthenticationUrl, UUserWidget*& BrowserWidget, bool bRememberMe)
{
	PendingSocialAuthenticationUrl = SocialAuthenticationUrl;

	// Check for user browser widget override
	auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

	auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
	MyBrowser->AddToViewport(MAX_int32);

	BrowserWidget = MyBrowser;

	// Be sure we've dropped any saved info
	LoginData = FXsollaLoginData();
	LoginData.bRememberMe = bRememberMe;
	SaveData();
}

void UXsollaLoginSubsystem::SetToken(const FString& token)
{
	LoginData.AuthToken.JWT = token;
	SaveData();
}

void UXsollaLoginSubsystem::UpdateUserAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectId));
	if (!UserId.IsEmpty())
	{
		RequestDataJson->SetStringField(TEXT("user_id"), UserId);
	}

	TArray<TSharedPtr<FJsonValue>> KeysJsonArray;
	for (auto Key : AttributeKeys)
	{
		KeysJsonArray.Push(MakeShareable(new FJsonValueString(Key)));
	}

	RequestDataJson->SetArrayField(TEXT("keys"), KeysJsonArray);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("%s/users/me/get"), *UserAttributesEndpoint);
	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UpdateUserAttributes_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ModifyUserAttributes(const FString& AuthToken, const TArray<FXsollaUserAttribute>& AttributesToModify, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());

	TArray<TSharedPtr<FJsonValue>> AttributesJsonArray;
	for (auto Attribute : AttributesToModify)
	{
		TSharedRef<FJsonObject> AttributeJson = MakeShareable(new FJsonObject());
		if (FJsonObjectConverter::UStructToJsonObject(FXsollaUserAttribute::StaticStruct(), &Attribute, AttributeJson, 0, 0))
		{
			AttributesJsonArray.Push(MakeShareable(new FJsonValueObject(AttributeJson)));
		}
	}

	RequestDataJson->SetArrayField(TEXT("attributes"), AttributesJsonArray);
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectId));

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("%s/users/me/update"), *UserAttributesEndpoint);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectId));
	SetStringArrayField(RequestDataJson, TEXT("removing_keys"), AttributesToRemove);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("%s/users/me/update"), *UserAttributesEndpoint);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaLoginSubsystem::UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
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
			//LoginData.bRememberMe = UGameplayStatics::ParseOption(UrlOptions, TEXT("remember_me")).ToBool();
			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);

			// Check if verification URL is provided
			if (Settings->VerifyTokenURL.IsEmpty())
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

void UXsollaLoginSubsystem::TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
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

void UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnSocialUrlReceived SuccessCallback, FOnAuthError ErrorCallback)
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
		static const FString SocialUrlFieldName = TEXT("url");
		if (JsonObject->HasTypedField<EJson::String>(SocialUrlFieldName))
		{
			FString SocialnUrl = JsonObject.Get()->GetStringField(SocialUrlFieldName);
			SuccessCallback.ExecuteIfBound(SocialnUrl);
			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *SocialUrlFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UpdateUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	TArray<FXsollaUserAttribute> userAttributesData;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseStr, &userAttributesData, 0, 0))
	{
		UserAttributes = userAttributesData;
		SuccessCallback.ExecuteIfBound();
		return;
	}

	// No success before so call the error callback
	FString ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

bool UXsollaLoginSubsystem::HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError ErrorCallback)
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

TSharedRef<IHttpRequest> UXsollaLoginSubsystem::CreateHttpRequest(const FString& Url, const FString& Content, const FString& AuthToken)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	// Temporal solution with headers processing on server-side #37
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=login&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_LOGIN_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	if (!Content.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetContentAsString(Content);
	}
	else
	{
		HttpRequest->SetVerb(TEXT("GET"));
	}

	if (!AuthToken.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));
	}

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("LOGIN"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_LOGIN_VERSION);

	return HttpRequest;
}

void UXsollaLoginSubsystem::SetStringArrayField(TSharedPtr<FJsonObject> Object, const FString& FieldName, const TArray<FString>& Array) const
{
	TArray<TSharedPtr<FJsonValue>> StringJsonArray;
	for (auto Item : Array)
	{
		StringJsonArray.Push(MakeShareable(new FJsonValueString(Item)));
	}

	Object->SetArrayField(FieldName, StringJsonArray);
}

bool UXsollaLoginSubsystem::ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject) const
{
	TArray<FString> TokenParts;
	Token.ParseIntoArray(TokenParts, TEXT("."));

	FString PayloadStr;
	if (!FBase64::Decode(TokenParts[1], PayloadStr))
	{
		return false;
	}

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PayloadStr);
	if (!FJsonSerializer::Deserialize(Reader, PayloadJsonObject))
	{
		return false;
	}

	return true;
}

FXsollaLoginData UXsollaLoginSubsystem::GetLoginData()
{
	return LoginData;
}

void UXsollaLoginSubsystem::DropLoginData()
{
	LoginData = FXsollaLoginData();

	// Drop saved data too
	UXsollaLoginSave::Save(LoginData);
}

FString UXsollaLoginSubsystem::GetUserId(const FString& Token)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return FString();
	}

	FString UserId;
	if (!PayloadJsonObject->TryGetStringField(TEXT("sub"), UserId))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Can't find user ID in token payload"), *VA_FUNC_LINE);
		return FString();
	}

	return UserId;
}

FString UXsollaLoginSubsystem::GetTokenProvider(const FString& token)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return FString();
	}

	FString Provider;
	if (!PayloadJsonObject->TryGetStringField(TEXT("provider"), Provider))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Can't find provider in token payload"), *VA_FUNC_LINE);
		return FString();
	}

	return Provider;
}

void UXsollaLoginSubsystem::LoadSavedData()
{
	LoginData = UXsollaLoginSave::Load();
}

void UXsollaLoginSubsystem::SaveData()
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

FString UXsollaLoginSubsystem::GetPendingSocialAuthenticationUrl() const
{
	return PendingSocialAuthenticationUrl;
}

TArray<FXsollaUserAttribute> UXsollaLoginSubsystem::GetUserAttributes()
{
	return UserAttributes;
}

UXsollaLoginSettings* UXsollaLoginSubsystem::GetSettings() const
{
	return Settings;
}

#undef LOCTEXT_NAMESPACE
