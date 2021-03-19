// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#include "XsollaLoginSubsystem.h"

#include "XsollaLogin.h"
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
#include "XsollaUtilsLibrary.h"

#if PLATFORM_ANDROID
#include "Android/XsollaJavaConvertor.h"
#include "Android/XsollaMethodCallUtils.h"
#include "Android/XsollaNativeAuthCallback.h"
#endif

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

const FString UXsollaLoginSubsystem::RegistrationEndpoint(TEXT("https://login.xsolla.com/api/user"));
const FString UXsollaLoginSubsystem::LoginEndpoint(TEXT("https://login.xsolla.com/api/login"));
const FString UXsollaLoginSubsystem::LoginSocialEndpoint(TEXT("https://login.xsolla.com/api/social"));
const FString UXsollaLoginSubsystem::ResetPasswordEndpoint(TEXT("https://login.xsolla.com/api/password/reset/request"));
const FString UXsollaLoginSubsystem::ProxyRegistrationEndpoint(TEXT("https://login.xsolla.com/api/proxy/registration"));
const FString UXsollaLoginSubsystem::ProxyLoginEndpoint(TEXT("https://login.xsolla.com/api/proxy/login"));
const FString UXsollaLoginSubsystem::ProxyResetPasswordEndpoint(TEXT("https://login.xsolla.com/api/proxy/password/reset"));
const FString UXsollaLoginSubsystem::ValidateTokenEndpoint(TEXT("https://login.xsolla.com/api/users/me"));
const FString UXsollaLoginSubsystem::UserAttributesEndpoint(TEXT("https://login.xsolla.com/api/attributes"));
const FString UXsollaLoginSubsystem::CrossAuthEndpoint(TEXT("https://login.xsolla.com/api/social"));
const FString UXsollaLoginSubsystem::AccountLinkingCodeEndpoint(TEXT("https://login.xsolla.com/api/users/account/code"));
const FString UXsollaLoginSubsystem::LoginEndpointOAuth(TEXT("https://login.xsolla.com/api/oauth2"));
const FString UXsollaLoginSubsystem::BlankRedirectEndpoint(TEXT("https://login.xsolla.com/api/blank"));
const FString UXsollaLoginSubsystem::UserDetailsEndpoint(TEXT("https://login.xsolla.com/api/users/me"));
const FString UXsollaLoginSubsystem::UsersEndpoint(TEXT("https://login.xsolla.com/api/users"));

UXsollaLoginSubsystem::UXsollaLoginSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/Xsolla/Login/Components/W_LoginBrowser.W_LoginBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
}

void UXsollaLoginSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSavedData();

	// Initialize subsystem with project identifiers provided by user
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	Initialize(Settings->ProjectID, Settings->LoginID);

	UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaLoginSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaLoginSubsystem::Initialize(const FString& InProjectId, const FString& InLoginId)
{
	ProjectID = InProjectId;
	LoginID = InLoginId;

	// Check token override from Xsolla Launcher
	const FString LauncherLoginJwt = UXsollaLoginLibrary::GetStringCommandLineParam(TEXT("xsolla-login-token"));
	if (!LauncherLoginJwt.IsEmpty())
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Xsolla Launcher login token is used"), *VA_FUNC_LINE);
		LoginData.AuthToken.JWT = LauncherLoginJwt;
	}

#if PLATFORM_ANDROID

	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	if (Settings->bAllowNativeAuth)
	{
		if (Settings->UseOAuth2)
		{
			XsollaMethodCallUtils::CallStaticVoidMethod("com/xsolla/login/XsollaNativeAuth", "xLoginInitOauth",
				"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
				FJavaWrapper::GameActivityThis,
				XsollaJavaConvertor::GetJavaString(LoginID),
				XsollaJavaConvertor::GetJavaString(Settings->ClientID),
				XsollaJavaConvertor::GetJavaString(Settings->CallbackURL),
				XsollaJavaConvertor::GetJavaString(Settings->FacebookAppId),
				XsollaJavaConvertor::GetJavaString(Settings->GoogleAppId));
		}
		else
		{
			XsollaMethodCallUtils::CallStaticVoidMethod("com/xsolla/login/XsollaNativeAuth", "xLoginInitJwt",
				"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
				FJavaWrapper::GameActivityThis,
				XsollaJavaConvertor::GetJavaString(LoginID),
				XsollaJavaConvertor::GetJavaString(Settings->CallbackURL),
				XsollaJavaConvertor::GetJavaString(Settings->FacebookAppId),
				XsollaJavaConvertor::GetJavaString(Settings->GoogleAppId));
		}
	}

#endif
}

void UXsollaLoginSubsystem::RegisterUser(const FString& Username, const FString& Password, const FString& Email, const FString& State,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM) && Settings->bUseSteamAuthorization)
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User registration should be handled via Steam"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Registration failed"), TEXT("User registration should be handled via Steam"));
		return;
	}

	if (Settings->UseOAuth2)
	{
		RegisterUserOAuth(Username, Password, Email, State, SuccessCallback, ErrorCallback);
	}
	else
	{
		RegisterUserJWT(Username, Password, Email, SuccessCallback, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::ResendAccountConfirmationEmail(const FString& Username, const FString& State, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (Settings->UseOAuth2)
	{
		ResendAccountConfirmationEmailOAuth(Username, State, SuccessCallback, ErrorCallback);
	}
	else
	{
		ResendAccountConfirmationEmailJWT(Username, SuccessCallback, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::AuthenticateUser(const FString& Username, const FString& Password,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, bool bRememberMe)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM) && Settings->bUseSteamAuthorization)
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

	if (Settings->UseOAuth2)
	{
		AuthenticateUserOAuth(Username, Password, SuccessCallback, ErrorCallback);
	}
	else
	{
		AuthenticateUserJWT(Username, Password, bRememberMe, SuccessCallback, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::ResetUserPassword(const FString& User, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM) && Settings->bUseSteamAuthorization)
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User password reset should be handled via Steam"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Password reset failed"), TEXT("User password reset should be handled via Steam"));
		return;
	}

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField((Settings->UserDataStorage == EUserDataStorage::Xsolla) ? TEXT("username") : TEXT("email"), User);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? ResetPasswordEndpoint : ProxyResetPasswordEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginID,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(ValidateTokenEndpoint, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), LoginData.AuthToken.JWT);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::TokenVerify_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetSocialAuthenticationUrl(const FString& ProviderName, const FString& State,
	const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (Settings->UseOAuth2)
	{
		GetSocialAuthenticationUrlOAuth(ProviderName, State, SuccessCallback, ErrorCallback);
	}
	else
	{
		GetSocialAuthenticationUrlJWT(ProviderName, SuccessCallback, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::LaunchSocialAuthentication(const FString& SocialAuthenticationUrl, UUserWidget*& BrowserWidget, bool bRememberMe)
{
	PendingSocialAuthenticationUrl = SocialAuthenticationUrl;

	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	// Check for user browser widget override
	const TSubclassOf<UUserWidget> BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

	auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
	MyBrowser->AddToViewport(MAX_int32);

	BrowserWidget = MyBrowser;

	// Be sure we've dropped any saved info
	LoginData = FXsollaLoginData();
	LoginData.bRememberMe = bRememberMe;
	SaveData();
}

void UXsollaLoginSubsystem::LaunchNativeSocialAuthentication(const FString& ProviderName, const FOnAuthUpdate& SuccessCallback,
	const FOnAuthCancel& CancelCallback, const FOnAuthError& ErrorCallback, bool bRememberMe)
{
#if PLATFORM_ANDROID

	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	if (Settings->bAllowNativeAuth)
	{
		UXsollaNativeAuthCallback* nativeCallback = NewObject<UXsollaNativeAuthCallback>();
		nativeCallback->BindSuccessDelegate(SuccessCallback);
		nativeCallback->BindCancelDelegate(CancelCallback);
		nativeCallback->BindErrorDelegate(ErrorCallback);

		XsollaMethodCallUtils::CallStaticVoidMethod("com/xsolla/login/XsollaNativeAuth", "xAuthSocial", "(Landroid/app/Activity;Ljava/lang/String;ZZJ)V",
			FJavaWrapper::GameActivityThis, XsollaJavaConvertor::GetJavaString(ProviderName), bRememberMe, Settings->InvalidateExistingSessions, (jlong)nativeCallback);
	}
	else
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Native authentication should be enabled in Project Settings"), *VA_FUNC_LINE);
	}

#endif
}

void UXsollaLoginSubsystem::AuthenticateViaProviderProject(const FString& AuthToken, const FString& PlatformProviderProject, const FString& Scope,
	const FOnAuthenticateViaProviderProjectSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("access_token"), AuthToken);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/cross/%s/login?client_id=%s&scope=%s"),
		*LoginEndpointOAuth,
		*PlatformProviderProject,
		*Settings->ClientID,
		*Scope);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetContentAsString(EncodeFormData(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::AuthenticateViaProviderProject_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::SetToken(const FString& Token)
{
	LoginData.AuthToken.JWT = Token;
	SaveData();
}

void UXsollaLoginSubsystem::RefreshToken(const FString& RefreshToken, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("client_id"), Settings->ClientID);
	RequestDataJson->SetStringField(TEXT("grant_type"), TEXT("refresh_token"));
	RequestDataJson->SetStringField(TEXT("refresh_token"), RefreshToken);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/token"), *LoginEndpointOAuth);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetContentAsString(EncodeFormData(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RefreshTokenOAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ExchangeAuthenticationCodeToToken(const FString& AuthenticationCode, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("client_id"), Settings->ClientID);
	RequestDataJson->SetStringField(TEXT("grant_type"), TEXT("authorization_code"));
	RequestDataJson->SetStringField(TEXT("code"), AuthenticationCode);
	RequestDataJson->SetStringField(TEXT("redirect_uri"), BlankRedirectEndpoint);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/token"), *LoginEndpointOAuth);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetContentAsString(EncodeFormData(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RefreshTokenOAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateWithSessionTicket(const FString& ProviderName, const FString& SessionTicket, const FString& AppId, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (Settings->UseOAuth2)
	{
		AuthenticateWithSessionTicketOAuth(ProviderName, AppId, SessionTicket, State, SuccessCallback, ErrorCallback);
	}
	else
	{
		AuthenticateWithSessionTicketJWT(ProviderName, AppId, SessionTicket, SuccessCallback, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::UpdateUserAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));
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
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("%s/users/me/get"), *UserAttributesEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UpdateUserAttributes_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateUserReadOnlyAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));
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

	const FString Url = FString::Printf(TEXT("%s/users/me/get_read_only"), *UserAttributesEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UpdateReadOnlyUserAttributes_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ModifyUserAttributes(const FString& AuthToken, const TArray<FXsollaUserAttribute>& AttributesToModify,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
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
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("%s/users/me/update"), *UserAttributesEndpoint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));
	SetStringArrayField(RequestDataJson, TEXT("removing_keys"), AttributesToRemove);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = FString::Printf(TEXT("%s/users/me/update"), *UserAttributesEndpoint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::CreateAccountLinkingCode(const FString& AuthToken, const FOnCodeReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(AccountLinkingCodeEndpoint, EXsollaLoginRequestVerb::VERB_POST, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::AccountLinkingCode_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::CheckUserAge(const FString& DateOfBirth, const FOnCheckUserAgeSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());

	RequestDataJson->SetStringField(TEXT("dob"), *DateOfBirth);
	RequestDataJson->SetStringField(TEXT("project_id"), *LoginID);

	const FString Url = FString::Printf(TEXT("%s/age/check"), *UsersEndpoint);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::CheckUserAge_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::LinkAccount(const FString& UserId, const EXsollaTargetPlatform Platform, const FString& Code,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString PlatformName = GetTargetPlatformName(Platform);
	const FString Url = FString::Printf(TEXT("%s?user_id=%s&platform=%s&code=%s"), *Settings->AccountLinkingURL, *UserId, *PlatformName, *Code);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticatePlatformAccountUser(const FString& UserId, const EXsollaTargetPlatform Platform,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString PlatformName = GetTargetPlatformName(Platform);
	const FString Url = FString::Printf(TEXT("%s?user_id=%s&platform=%s&with_logout=%s"),
		*Settings->PlatformAuthenticationURL,
		*UserId,
		*PlatformName,
		Settings->InvalidateExistingSessions ? TEXT("1") : TEXT("0"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::AuthConsoleAccountUser_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetwork(
	const FString& AuthToken, const FString& AuthTokenSecret,
	const FString& ProviderName, const FString& Payload, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	if (IOnlineSubsystem::IsEnabled(STEAM_SUBSYSTEM) && Settings->bUseSteamAuthorization)
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User registration should be handled via Steam"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Registration failed"), TEXT("User registration should be handled via Steam"));
		return;
	}

	if (Settings->UseOAuth2)
	{
		AuthViaAccessTokenOfSocialNetworkOAuth(AuthToken, AuthTokenSecret, ProviderName, State, SuccessCallback, ErrorCallback);
	}
	else
	{
		AuthViaAccessTokenOfSocialNetworkJWT(AuthToken, AuthTokenSecret, ProviderName, Payload, SuccessCallback, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::UpdateUserDetails(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(UserDetailsEndpoint, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserDetails_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ModifyUserDetails(const FString& AuthToken, const FString& Birthday, const FString& FirstName, const FString& LastName,
	const FString& Gender, const FString& Nickname, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());

	if (!Birthday.IsEmpty())
		RequestDataJson->SetStringField(TEXT("birthday"), Birthday);
	if (!Gender.IsEmpty())
		RequestDataJson->SetStringField(TEXT("gender"), Gender);

	RequestDataJson->SetStringField(TEXT("first_name"), FirstName);
	RequestDataJson->SetStringField(TEXT("last_name"), LastName);
	RequestDataJson->SetStringField(TEXT("nickname"), Nickname);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(UserDetailsEndpoint, EXsollaLoginRequestVerb::VERB_PATCH, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserDetails_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateUserEmail(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/email"), *UserDetailsEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserEmail_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateUserPhoneNumber(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/phone"), *UserDetailsEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserPhoneNumber_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ModifyUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("phone_number"), PhoneNumber);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/phone"), *UserDetailsEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::ModifyPhoneNumber_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::RemoveUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/phone/%s"),
		*UserDetailsEndpoint,
		PhoneNumber.IsEmpty() ? *UserDetails.phone : *PhoneNumber);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_DELETE, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RemovePhoneNumber_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ModifyUserProfilePicture(const FString& AuthToken, UTexture2D* Picture,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	if (!IsValid(Picture))
	{
		ErrorCallback.Execute("-1", "Picture is invalid.");
		return;
	}

	// Prepare picture upload request content
	const FString Boundary = TEXT("---------------------------" + FString::FromInt(FDateTime::Now().GetTicks()));
	const FString BeginBoundary = TEXT("\r\n--" + Boundary + "\r\n");
	const FString EndBoundary = TEXT("\r\n--" + Boundary + "--\r\n");

	FString PictureHeader = TEXT("Content-Disposition: form-data;");
	PictureHeader.Append(TEXT("name=\"picture\";"));
	PictureHeader.Append(TEXT("filename=\"avatar.png\""));
	PictureHeader.Append(TEXT("\r\nContent-Type: \r\n\r\n"));

	TArray<uint8> UploadContent;
	UploadContent.Append((uint8*)TCHAR_TO_ANSI(*BeginBoundary), BeginBoundary.Len());
	UploadContent.Append((uint8*)TCHAR_TO_ANSI(*PictureHeader), PictureHeader.Len());
	UploadContent.Append(UXsollaLoginLibrary::ConvertTextureToByteArray(Picture));
	UploadContent.Append((uint8*)TCHAR_TO_ANSI(*EndBoundary), EndBoundary.Len());

	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/picture"), *UserDetailsEndpoint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, TEXT(""), AuthToken);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary =" + Boundary));
	HttpRequest->SetContent(UploadContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserProfilePicture_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::RemoveProfilePicture(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/picture"), *UserDetailsEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_DELETE, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserProfilePictureRemove_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateFriends(const FString& AuthToken, EXsollaFriendsType Type, EXsollaUsersSortCriteria SortBy, EXsollaUsersSortOrder SortOrder,
	const FOnUserFriendsUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const FString FriendType = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaFriendsType", Type);
	const FString SortByCriteria = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaUsersSortCriteria", SortBy);
	const FString SortOrderCriteria = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaUsersSortOrder", SortOrder);

	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/relationships?type=%s&sort_by=%s&sort_order=%s"),
		*UserDetailsEndpoint,
		*FriendType,
		*SortByCriteria,
		*SortOrderCriteria);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserFriends_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ModifyFriends(const FString& AuthToken, EXsollaFriendAction Action, const FString& UserID,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("user"), UserID);
	RequestDataJson->SetStringField(TEXT("action"), UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaFriendAction", Action));

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/relationships"), *UserDetailsEndpoint);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent, AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateSocialAuthLinks(const FString& AuthToken, const FString& Locale, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/login_urls%s"),
		*UserDetailsEndpoint,
		Locale.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("?locale=%s"), *Locale));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAuthLinks_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateSocialFriends(const FString& AuthToken, const FString& Platform,
	const FOnUserSocialFriendsUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, int Offset, int Limit, bool FromThisGame)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/social_friends?limit=%d&offset=%d&with_xl_uid=%s%s"),
		*UserDetailsEndpoint,
		Limit,
		Offset,
		FromThisGame ? TEXT("true") : TEXT("false"),
		Platform.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("&platform=%s"), *Platform));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialFriends_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateUsersFriends(const FString& AuthToken, const FString& Platform, const FOnCodeReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/social_friends/update%s"),
		*UserDetailsEndpoint,
		Platform.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("?platform=%s"), *Platform));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UpdateUsersFriends_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetAccessTokenFromCustomAuthServer(const FXsollaParameters Parameters,
	const FOnAccessTokenLoginSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();

	const FString Url = FString::Printf(TEXT("%slogin"), *Settings->CustomAuthServerURL);

	const TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject);
	UXsollaUtilsLibrary::AddParametersToJsonObject(RequestDataJson, Parameters);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this,
		&UXsollaLoginSubsystem::GetAccessTokenByEmail_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetUserProfile(const FString& AuthToken, const FString& UserID, const FOnUserProfileReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/%s/public"),
		*UsersEndpoint,
		*UserID);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserProfile_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::SearchUsersByNickname(const FString& AuthToken, const FString& Nickname,
	const FOnUserSearchUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, int Offset, int Limit)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/search/by_nickname?nickname=%s&limit=%d&offset=%d"),
		*UsersEndpoint,
		*FGenericPlatformHttp::UrlEncode(Nickname),
		Limit,
		Offset);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserSearch_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::LinkSocialNetworkToUserAccount(const FString& AuthToken, const FString& ProviderName,
	const FOnSocialAccountLinkingHtmlReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/me/social_providers/%s/login_url?login_url=%s"),
		*UsersEndpoint,
		*ProviderName,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAccountLinking_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::UpdateLinkedSocialNetworks(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = FString::Printf(TEXT("%s/me/social_providers"), *UsersEndpoint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET, TEXT(""), AuthToken);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::LinkedSocialNetworks_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::RegisterUserJWT(const FString& Username, const FString& Password, const FString& Email,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetStringField(TEXT("email"), Email);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? RegistrationEndpoint : ProxyRegistrationEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginID,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::RegisterUserOAuth(const FString& Username, const FString& Password, const FString& Email, const FString& State,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetStringField(TEXT("email"), Email);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? RegistrationEndpoint : ProxyRegistrationEndpoint;
	const FString Url = FString::Printf(TEXT("%s/user?response_type=code&client_id=%s&state=%s&redirect_uri=%s"),
		*LoginEndpointOAuth,
		*Settings->ClientID,
		*State,
		*BlankRedirectEndpoint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ResendAccountConfirmationEmailJWT(const FString& Username, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? RegistrationEndpoint : ProxyRegistrationEndpoint;
	const FString Url = FString::Printf(TEXT("%s/resend_confirmation_link?projectId=%s&login_url=%s"),
		*Endpoint,
		*LoginID,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ResendAccountConfirmationEmailOAuth(const FString& Username, const FString& State, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? RegistrationEndpoint : ProxyRegistrationEndpoint;
	const FString Url = FString::Printf(TEXT("%s/user/resend_confirmation_link?client_id=%s&state=%s&redirect_uri=%s"),
		*LoginEndpointOAuth,
		*Settings->ClientID,
		*State,
		*BlankRedirectEndpoint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateUserJWT(const FString& Username, const FString& Password, bool bRememberMe,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetBoolField(TEXT("remember_me"), bRememberMe);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Endpoint = (Settings->UserDataStorage == EUserDataStorage::Xsolla) ? LoginEndpoint : ProxyLoginEndpoint;
	const FString Url = FString::Printf(TEXT("%s?projectId=%s&login_url=%s&with_logout=%s"),
		*Endpoint,
		*LoginID,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL),
		Settings->InvalidateExistingSessions ? TEXT("1") : TEXT("0"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserLogin_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateUserOAuth(const FString& Username, const FString& Password, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/login/token?client_id=%s&scope=offline"),
		*LoginEndpointOAuth,
		*Settings->ClientID);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserLoginOAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetSocialAuthenticationUrlJWT(const FString& ProviderName, const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/%s/login_url?projectId=%s&login_url=%s&with_logout=%s"),
		*LoginSocialEndpoint,
		*ProviderName,
		*LoginID,
		*FGenericPlatformHttp::UrlEncode(Settings->CallbackURL),
		Settings->InvalidateExistingSessions ? TEXT("1") : TEXT("0"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetSocialAuthenticationUrlOAuth(const FString& ProviderName, const FString& State,
	const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/social/%s/login_url?client_id=%s&redirect_uri=%s&response_type=code&state=%s&scope=offline"),
		*LoginEndpointOAuth,
		*ProviderName,
		*Settings->ClientID,
		*BlankRedirectEndpoint,
		*State);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateWithSessionTicketJWT(const FString& ProviderName, const FString& AppId, const FString& SessionTicket,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/%s/cross_auth?projectId=%s&app_id=%s&session_ticket=%s&is_redirect=false&with_logout=%s"),
		*CrossAuthEndpoint,
		*ProviderName,
		*LoginID,
		*AppId,
		*SessionTicket,
		Settings->InvalidateExistingSessions ? TEXT("1") : TEXT("0"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::CrossAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateWithSessionTicketOAuth(const FString& ProviderName, const FString& AppId, const FString& SessionTicket, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/social/%s/cross_auth?client_id=%s&response_type=code&redirect_uri=%s&state=%s&app_id=%s&scope=offline&session_ticket=%s&is_redirect=false"),
		*LoginEndpointOAuth,
		*ProviderName,
		*Settings->ClientID,
		*BlankRedirectEndpoint,
		*State,
		*AppId,
		*SessionTicket);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SessionTicketOAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetworkJWT(const FString& AuthToken, const FString& AuthTokenSecret, const FString& ProviderName,
	const FString& Payload, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/%s/login_with_token?projectId=%s&payload=%s&with_logout=%s"),
		*LoginSocialEndpoint,
		*ProviderName,
		*LoginID,
		*Payload,
		Settings->InvalidateExistingSessions ? TEXT("1") : TEXT("0"));

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("access_token"), *AuthToken);
	if (ProviderName.Compare(TEXT("twitter")))
	{
		RequestDataJson->SetStringField(TEXT("access_token_secret"), *AuthTokenSecret);
	}

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetworkJWT_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetworkOAuth(
	const FString& AuthToken, const FString& AuthTokenSecret,
	const FString& ProviderName, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaLoginSettings* Settings = FXsollaLoginModule::Get().GetSettings();
	const FString Url = FString::Printf(TEXT("%s/social/%s/login_with_token?client_id=%s&response_type=code&redirect_uri=%s&state=%s&scope=offline"),
		*LoginEndpointOAuth,
		*ProviderName,
		*Settings->ClientID,
		*BlankRedirectEndpoint,
		*State);

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("access_token"), *AuthToken);
	if (ProviderName.Compare(TEXT("twitter")))
	{
		RequestDataJson->SetStringField(TEXT("access_token_secret"), *AuthTokenSecret);
	}

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaLoginRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetworkOAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound();
}

void UXsollaLoginSubsystem::UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString LoginUrlFieldName = TEXT("login_url");
		if (JsonObject->HasTypedField<EJson::String>(LoginUrlFieldName))
		{
			const FString LoginUrl = JsonObject.Get()->GetStringField(LoginUrlFieldName);
			const FString UrlOptions = LoginUrl.RightChop(LoginUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));

			LoginData.AuthToken.JWT = UGameplayStatics::ParseOption(UrlOptions, TEXT("token"));

			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);

			SuccessCallback.ExecuteIfBound(LoginData);

			return;
		}
		ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *LoginUrlFieldName);
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UserLoginOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleOAuthTokenRequest(HttpRequest, HttpResponse, bSucceeded, ErrorCallback, SuccessCallback);
}

void UXsollaLoginSubsystem::TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	// If no error happened so token is verified now
	LoginData.AuthToken.bIsVerified = true;
	SaveData();

	SuccessCallback.ExecuteIfBound(LoginData);
}

void UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnSocialUrlReceived SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString SocialUrlFieldName = TEXT("url");
		if (JsonObject->HasTypedField<EJson::String>(SocialUrlFieldName))
		{
			const FString SocialnUrl = JsonObject.Get()->GetStringField(SocialUrlFieldName);
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

void UXsollaLoginSubsystem::CrossAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString LoginUrlFieldName = TEXT("login_url");
		if (JsonObject->HasTypedField<EJson::String>(LoginUrlFieldName))
		{
			const FString LoginUrlRaw = JsonObject.Get()->GetStringField(LoginUrlFieldName);
			const FString LoginUrl = FGenericPlatformHttp::UrlDecode(LoginUrlRaw);
			const FString UrlOptions = LoginUrl.RightChop(LoginUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));

			const FString Token = UGameplayStatics::ParseOption(UrlOptions, TEXT("token"));

			LoginData.AuthToken.JWT = Token;

			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);

			SuccessCallback.ExecuteIfBound(LoginData);

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

void UXsollaLoginSubsystem::UpdateUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	TArray<FXsollaUserAttribute> userAttributesData;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseStr, &userAttributesData, 0, 0))
	{
		UserAttributes = userAttributesData;
		SuccessCallback.ExecuteIfBound();
		return;
	}

	// No success before so call the error callback
	const FString ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UpdateReadOnlyUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	TArray<FXsollaUserAttribute> userReadOnlyAttributesData;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseStr, &userReadOnlyAttributesData, 0, 0))
	{
		UserReadOnlyAttributes = userReadOnlyAttributesData;
		SuccessCallback.ExecuteIfBound();
		return;
	}

	// No success before so call the error callback
	const FString ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::AccountLinkingCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnCodeReceived SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString AccountLinkingCode = TEXT("code");
		if (JsonObject->HasTypedField<EJson::String>(AccountLinkingCode))
		{
			const FString Code = JsonObject.Get()->GetStringField(AccountLinkingCode);
			SuccessCallback.ExecuteIfBound(Code);
			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *AccountLinkingCode);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::CheckUserAge_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnCheckUserAgeSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaCheckUserAgeResult CheckUserAgeResult;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaCheckUserAgeResult::StaticStruct(), &CheckUserAgeResult))
		{
			SuccessCallback.ExecuteIfBound(CheckUserAgeResult);
			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json"));
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::AuthConsoleAccountUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString TokenFieldName = TEXT("token");
		if (JsonObject->HasTypedField<EJson::String>(TokenFieldName))
		{
			const FString Token = JsonObject.Get()->GetStringField(TokenFieldName);

			LoginData.AuthToken.JWT = Token;

			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);

			SuccessCallback.ExecuteIfBound(LoginData);

			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *TokenFieldName);
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::AuthenticateViaProviderProject_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthenticateViaProviderProjectSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaProviderToken ProviderToken;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaProviderToken::StaticStruct(), &ProviderToken))
		{
			LoginData.AuthToken.JWT = ProviderToken.access_token;
			LoginData.AuthToken.RefreshToken = ProviderToken.refresh_token;
			LoginData.AuthToken.ExpiresAt = FDateTime::UtcNow().ToUnixTimestamp() + ProviderToken.expires_in;

			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);
			SuccessCallback.ExecuteIfBound(ProviderToken);
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::RefreshTokenOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleOAuthTokenRequest(HttpRequest, HttpResponse, bSucceeded, ErrorCallback, SuccessCallback);
}

void UXsollaLoginSubsystem::SessionTicketOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString LoginUrlFieldName = TEXT("login_url");
		if (JsonObject->HasTypedField<EJson::String>(LoginUrlFieldName))
		{
			const FString LoginUrlRaw = JsonObject.Get()->GetStringField(LoginUrlFieldName);
			const FString LoginUrl = FGenericPlatformHttp::UrlDecode(LoginUrlRaw);
			const FString UrlOptions = LoginUrl.RightChop(LoginUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));

			const FString Code = UGameplayStatics::ParseOption(UrlOptions, TEXT("code"));

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received code: %s"), *VA_FUNC_LINE, *Code);

			ExchangeAuthenticationCodeToToken(Code, SuccessCallback, ErrorCallback);

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *LoginUrlFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetworkJWT_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const FString TokenField = TEXT("token");
		if (JsonObject->HasTypedField<EJson::String>(TokenField))
		{
			const FString Token = JsonObject->GetStringField(TokenField);

			LoginData.AuthToken.JWT = Token;
			LoginData.Username = GetTokenParameter(Token, "username");

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *Token);

			SuccessCallback.ExecuteIfBound(LoginData);
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *TokenField);
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetworkOAuth_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const FString LoginUrlFieldName = TEXT("login_url");
		if (JsonObject->HasTypedField<EJson::String>(LoginUrlFieldName))
		{
			const FString LoginUrlRaw = JsonObject.Get()->GetStringField(LoginUrlFieldName);
			const FString LoginUrl = FGenericPlatformHttp::UrlDecode(LoginUrlRaw);
			const FString UrlOptions = LoginUrl.RightChop(LoginUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));

			const FString Code = UGameplayStatics::ParseOption(UrlOptions, TEXT("code"));

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received code: %s"), *VA_FUNC_LINE, *Code);

			ExchangeAuthenticationCodeToToken(Code, SuccessCallback, ErrorCallback);

			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *LoginUrlFieldName);
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UserDetails_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaUserDetails receivedUserDetails;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaUserDetails::StaticStruct(), &receivedUserDetails))
		{
			UserDetails = receivedUserDetails;

			SuccessCallback.ExecuteIfBound();

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json"));
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UserEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
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
		static const FString EmailFieldName = TEXT("current_email");
		if (JsonObject->HasTypedField<EJson::String>(EmailFieldName))
		{
			UserDetails.email = JsonObject->GetStringField(EmailFieldName);

			SuccessCallback.ExecuteIfBound();

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *EmailFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UserPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
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
		static const FString PhoneFieldName = TEXT("phone_number");
		if (JsonObject->HasTypedField<EJson::String>(PhoneFieldName))
		{
			UserDetails.phone = JsonObject->GetStringField(PhoneFieldName);

			SuccessCallback.ExecuteIfBound();

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *PhoneFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::ModifyPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const TArray<uint8> Content = HttpRequest->GetContent();
	FUTF8ToTCHAR Converter((ANSICHAR*)Content.GetData(), Content.Num());
	FString ContentStr = FString(Converter.Length(), Converter.Get());

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ContentStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString PhoneFieldName = TEXT("phone_number");
		if (JsonObject->HasTypedField<EJson::String>(PhoneFieldName))
		{
			UserDetails.phone = JsonObject->GetStringField(PhoneFieldName);

			SuccessCallback.ExecuteIfBound();

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *PhoneFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ContentStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::RemovePhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	UserDetails.phone = TEXT("");

	SuccessCallback.ExecuteIfBound();
}

void UXsollaLoginSubsystem::UserProfilePicture_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
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
		static const FString PictureFieldName = TEXT("picture");
		if (JsonObject->HasTypedField<EJson::String>(PictureFieldName))
		{
			UserDetails.picture = JsonObject->GetStringField(PictureFieldName);

			SuccessCallback.ExecuteIfBound();

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *PictureFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UserProfilePictureRemove_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	UserDetails.picture = TEXT("");

	SuccessCallback.ExecuteIfBound();
}

void UXsollaLoginSubsystem::UserFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnUserFriendsUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaFriendsData receivedUserFriendsData;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaFriendsData::StaticStruct(), &receivedUserFriendsData))
		{
			FString RequestUrl = HttpRequest->GetURL();
			FString UrlOptions = RequestUrl.RightChop(RequestUrl.Find(TEXT("?"))).Replace(TEXT("&"), TEXT("?"));
			FString Type = UGameplayStatics::ParseOption(UrlOptions, TEXT("type"));

			SuccessCallback.ExecuteIfBound(receivedUserFriendsData, UXsollaUtilsLibrary::GetEnumValueFromString<EXsollaFriendsType>("EXsollaFriendsType", Type));

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json"));
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::SocialAuthLinks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	TArray<FXsollaSocialAuthLink> socialAuthLinksData;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseStr, &socialAuthLinksData, 0, 0))
	{
		SocialAuthLinks = socialAuthLinksData;
		SuccessCallback.ExecuteIfBound();
		return;
	}

	// No success before so call the error callback
	FString ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::SocialFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnUserSocialFriendsUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaSocialFriendsData receivedUserSocialFriendsData;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaSocialFriendsData::StaticStruct(), &receivedUserSocialFriendsData))
		{
			SocialFriendsData = receivedUserSocialFriendsData;

			SuccessCallback.ExecuteIfBound(SocialFriendsData);

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json"));
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UpdateUsersFriends_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnCodeReceived SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	SuccessCallback.ExecuteIfBound(TEXT("204"));
}

void UXsollaLoginSubsystem::UserProfile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnUserProfileReceived SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaPublicProfile receivedUserProfile;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaPublicProfile::StaticStruct(), &receivedUserProfile))
		{
			SuccessCallback.ExecuteIfBound(receivedUserProfile);

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json"));
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::UserSearch_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnUserSearchUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaLogin, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaUserSearchResult searchResult;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaUserSearchResult::StaticStruct(), &searchResult))
		{
			SuccessCallback.ExecuteIfBound(searchResult);

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json"));
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::SocialAccountLinking_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnSocialAccountLinkingHtmlReceived SuccessCallback, FOnAuthError ErrorCallback)
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

void UXsollaLoginSubsystem::LinkedSocialNetworks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	TArray<FXsollaLinkedSocialNetworkData> socialNetworks;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(ResponseStr, &socialNetworks, 0, 0))
	{
		LinkedSocialNetworks = socialNetworks;
		SuccessCallback.ExecuteIfBound();
		return;
	}

	// No success before so call the error callback
	const FString ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::GetAccessTokenByEmail_HttpRequestComplete(
	const FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnAccessTokenLoginSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		static const FString AccessTokenFieldName = TEXT("access_token");
		if (JsonObject->HasTypedField<EJson::String>(AccessTokenFieldName))
		{
			const FString AccessToken = JsonObject->GetStringField(AccessTokenFieldName);

			SuccessCallback.ExecuteIfBound(AccessToken);
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *AccessTokenFieldName);
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	ErrorCallback.ExecuteIfBound(TEXT("204"), ErrorStr);
}

void UXsollaLoginSubsystem::HandleOAuthTokenRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnAuthError& ErrorCallback, FOnAuthUpdate& SuccessCallback)
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
		static const FString AccessTokenFieldName = TEXT("access_token");
		if (JsonObject->HasTypedField<EJson::String>(AccessTokenFieldName))
		{
			LoginData.AuthToken.JWT = JsonObject->GetStringField(AccessTokenFieldName);
			LoginData.AuthToken.RefreshToken = JsonObject->GetStringField(TEXT("refresh_token"));
			LoginData.AuthToken.ExpiresAt = FDateTime::UtcNow().ToUnixTimestamp() + JsonObject->GetNumberField(TEXT("expires_in"));

			SaveData();

			UE_LOG(LogXsollaLogin, Log, TEXT("%s: Received token: %s"), *VA_FUNC_LINE, *LoginData.AuthToken.JWT);

			SuccessCallback.ExecuteIfBound(LoginData);

			return;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Can't process response json: no field '%s' found"), *AccessTokenFieldName);
		}
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: "), *ResponseStr);
	}

	// No success before so call the error callback
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

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaLoginSubsystem::CreateHttpRequest(const FString& Url, const EXsollaLoginRequestVerb Verb, const FString& Content, const FString& AuthToken)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Temporal solution with headers processing on server-side #37
	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=login&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_LOGIN_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	switch (Verb)
	{
	case EXsollaLoginRequestVerb::VERB_GET:
		HttpRequest->SetVerb(TEXT("GET"));

		// Check that we doen't provide content with GET request
		if (!Content.IsEmpty())
		{
			UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Request content is not empty for GET request. Maybe you should use POST one?"), *VA_FUNC_LINE);
		}
		break;

	case EXsollaLoginRequestVerb::VERB_POST:
		HttpRequest->SetVerb(TEXT("POST"));
		break;

	case EXsollaLoginRequestVerb::VERB_PUT:
		HttpRequest->SetVerb(TEXT("PUT"));
		break;

	case EXsollaLoginRequestVerb::VERB_DELETE:
		HttpRequest->SetVerb(TEXT("DELETE"));
		break;

	case EXsollaLoginRequestVerb::VERB_PATCH:
		HttpRequest->SetVerb(TEXT("PATCH"));
		break;

	default:
		unimplemented();
	}

	if (!Content.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		HttpRequest->SetContentAsString(Content);
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
	UXsollaUtilsLibrary::PatchRequestWithPartnerInfo(HttpRequest);

	return HttpRequest;
}

FString UXsollaLoginSubsystem::EncodeFormData(TSharedPtr<FJsonObject> FormDataJson)
{
	FString EncodedFormData = "";
	uint16 ParamIndex = 0;

	for (auto FormDataIt = FormDataJson->Values.CreateIterator(); FormDataIt; ++FormDataIt)
	{
		FString Key = FormDataIt.Key();
		FString Value = FormDataIt.Value().Get()->AsString();

		if (!Key.IsEmpty() && !Value.IsEmpty())
		{
			EncodedFormData += ParamIndex == 0 ? "" : "&";
			EncodedFormData += FGenericPlatformHttp::UrlEncode(Key) + "=" + FGenericPlatformHttp::UrlEncode(Value);
		}

		ParamIndex++;
	}

	return EncodedFormData;
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
	if (TokenParts.Num() <= 1)
	{
		return false;
	}

	FString PayloadStr;
	if (!FBase64::Decode(TokenParts[1], PayloadStr))
	{
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PayloadStr);
	if (!FJsonSerializer::Deserialize(Reader, PayloadJsonObject))
	{
		return false;
	}

	return true;
}

FString UXsollaLoginSubsystem::GetTargetPlatformName(EXsollaTargetPlatform Platform)
{
	FString platform;

	switch (Platform)
	{
	case EXsollaTargetPlatform::PlaystationNetwork:
		platform = TEXT("playstation_network");
		break;

	case EXsollaTargetPlatform::XboxLive:
		platform = TEXT("xbox_live");
		break;

	case EXsollaTargetPlatform::Xsolla:
		platform = TEXT("xsolla");
		break;

	case EXsollaTargetPlatform::PcStandalone:
		platform = TEXT("pc_standalone");
		break;

	case EXsollaTargetPlatform::NintendoShop:
		platform = TEXT("nintendo_shop");
		break;

	case EXsollaTargetPlatform::GooglePlay:
		platform = TEXT("google_play");
		break;

	case EXsollaTargetPlatform::AppStoreIos:
		platform = TEXT("app_store_ios");
		break;

	case EXsollaTargetPlatform::AndroidStandalone:
		platform = TEXT("android_standalone");
		break;

	case EXsollaTargetPlatform::IosStandalone:
		platform = TEXT("ios_standalone");
		break;

	case EXsollaTargetPlatform::AndroidOther:
		platform = TEXT("android_other");
		break;

	case EXsollaTargetPlatform::IosOther:
		platform = TEXT("ios_other");
		break;

	case EXsollaTargetPlatform::PcOther:
		platform = TEXT("pc_other");
		break;

	default:
		platform = TEXT("");
	}

	return platform;
}

FXsollaLoginData UXsollaLoginSubsystem::GetLoginData()
{
	return LoginData;
}

void UXsollaLoginSubsystem::SetLoginData(const FXsollaLoginData& Data, bool ClearCache)
{
	// Refresh saved data in memory
	LoginData = Data;

	if (ClearCache)
	{
		// Drop saved data in cache
		UXsollaLoginSave::Save(LoginData);
	}
}

void UXsollaLoginSubsystem::DropLoginData(bool ClearCache)
{
	// Drop saved data in memory
	LoginData = FXsollaLoginData();

	if (ClearCache)
	{
		// Drop saved data in cache
		UXsollaLoginSave::Save(LoginData);
	}
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

FString UXsollaLoginSubsystem::GetTokenProvider(const FString& Token)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
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

FString UXsollaLoginSubsystem::GetTokenParameter(const FString& Token, const FString& Parameter)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaLogin, Log, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return FString();
	}

	FString ParameterValue;
	if (!PayloadJsonObject->TryGetStringField(Parameter, ParameterValue))
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Can't find parameter %s in token payload"), *VA_FUNC_LINE, *Parameter);
		return FString();
	}

	return ParameterValue;
}

bool UXsollaLoginSubsystem::IsMasterAccount(const FString& Token)
{
	TSharedPtr<FJsonObject> PayloadJsonObject;
	if (!ParseTokenPayload(Token, PayloadJsonObject))
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Can't parse token payload"), *VA_FUNC_LINE);
		return false;
	}

	bool ParameterValue = false;
	if (!PayloadJsonObject->TryGetBoolField(TEXT("is_master"), ParameterValue))
	{
		return false;
	}

	return ParameterValue;
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
		// Don't drop cache in memory but reset save file
		UXsollaLoginSave::Save(FXsollaLoginData());
	}
}

FString UXsollaLoginSubsystem::GetPendingSocialAuthenticationUrl() const
{
	return PendingSocialAuthenticationUrl;
}

FString UXsollaLoginSubsystem::GetSocialAccountLinkingHtml() const
{
	return SocialAccountLinkingHtml;
}

TArray<FXsollaUserAttribute> UXsollaLoginSubsystem::GetUserAttributes()
{
	return UserAttributes;
}

TArray<FXsollaUserAttribute> UXsollaLoginSubsystem::GetUserReadOnlyAttributes()
{
	return UserReadOnlyAttributes;
}

FXsollaUserDetails UXsollaLoginSubsystem::GetUserDetails() const
{
	return UserDetails;
}

TArray<FXsollaSocialAuthLink> UXsollaLoginSubsystem::GetSocialAuthLinks() const
{
	return SocialAuthLinks;
}

FXsollaSocialFriendsData UXsollaLoginSubsystem::GetSocialFriends() const
{
	return SocialFriendsData;
}

TArray<FXsollaSocialFriend> UXsollaLoginSubsystem::GetSocialProfiles(const FString& UserID) const
{
	auto SocialProfiles = SocialFriendsData.data.FilterByPredicate([UserID](const FXsollaSocialFriend& InSocialProfile) {
		return InSocialProfile.xl_uid == UserID;
	});

	return SocialProfiles;
}

TArray<FXsollaLinkedSocialNetworkData> UXsollaLoginSubsystem::GetLinkedSocialNetworks() const
{
	return LinkedSocialNetworks;
}

bool UXsollaLoginSubsystem::IsSocialNetworkLinked(const FString& Provider) const
{
	auto SocialNetwork = LinkedSocialNetworks.FindByPredicate([Provider](const FXsollaLinkedSocialNetworkData& InSocialNetwork) {
		return InSocialNetwork.provider == Provider;
	});

	return SocialNetwork != nullptr;
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_xsolla_login_XsollaNativeAuthActivity_onAuthSuccessCallback(JNIEnv* env, jclass clazz, jlong objAddr,
	jstring accessToken, jstring refreshToken, jlong expiresAt, jboolean rememberMe)
{
	UXsollaNativeAuthCallback* callback = reinterpret_cast<UXsollaNativeAuthCallback*>(objAddr);

	if (IsValid(callback))
	{
		FXsollaLoginData receivedData;
		receivedData.AuthToken.JWT = XsollaJavaConvertor::FromJavaString(accessToken);
		receivedData.AuthToken.RefreshToken = XsollaJavaConvertor::FromJavaString(refreshToken);
		receivedData.AuthToken.ExpiresAt = (int64)expiresAt;
		receivedData.bRememberMe = rememberMe;
		callback->ExecuteSuccess(receivedData);
	}
	else
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Invalid callback"), *VA_FUNC_LINE);
	}
}

JNI_METHOD void Java_com_xsolla_login_XsollaNativeAuthActivity_onAuthCancelCallback(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UXsollaNativeAuthCallback* callback = reinterpret_cast<UXsollaNativeAuthCallback*>(objAddr);

	if (IsValid(callback))
	{
		callback->ExecuteCancel();
	}
	else
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Invalid callback"), *VA_FUNC_LINE);
	}
}

JNI_METHOD void Java_com_xsolla_login_XsollaNativeAuthActivity_onAuthErrorCallback(JNIEnv* env, jclass clazz, jlong objAddr, jstring errorMsg)
{
	UXsollaNativeAuthCallback* callback = reinterpret_cast<UXsollaNativeAuthCallback*>(objAddr);

	if (IsValid(callback))
	{
		callback->ExecuteError(XsollaJavaConvertor::FromJavaString(errorMsg));
	}
	else
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: Invalid callback"), *VA_FUNC_LINE);
	}
}

#endif

#undef LOCTEXT_NAMESPACE
