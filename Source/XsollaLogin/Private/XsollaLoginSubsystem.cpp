// Copyright 2021 Xsolla Inc. All Rights Reserved.

#include "XsollaLoginSubsystem.h"

#include "XsollaLogin.h"
#include "XsollaLoginDefines.h"
#include "XsollaLoginLibrary.h"
#include "XsollaLoginSave.h"
#include "XsollaUtilsLibrary.h"
#include "XsollaUtilsTokenParser.h"
#include "XsollaUtilsUrlBuilder.h"

#include "Dom/JsonObject.h"
#include "Engine/Engine.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "UObject/ConstructorHelpers.h"
#include "XsollaSettingsModule.h"
#include "XsollaProjectSettings.h"
#include "XsollaUtilsDataModel.h"

#if PLATFORM_ANDROID
#include "Android/XsollaJavaConvertor.h"
#include "Android/XsollaMethodCallUtils.h"
#include "Android/XsollaNativeAuthCallback.h"
#endif

#if PLATFORM_IOS
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AuthenticationServices/AuthenticationServices.h>
#import <XsollaSDKLoginKitObjectiveC/XsollaSDKLoginKitObjectiveC-Swift.h>
#endif

#define LOCTEXT_NAMESPACE "FXsollaLoginModule"

UXsollaLoginSubsystem::UXsollaLoginSubsystem()
	: UGameInstanceSubsystem()
{
#if !UE_SERVER
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(*FString::Printf(TEXT("/%s/Login/Components/W_LoginBrowser.W_LoginBrowser_C"),
		*UXsollaUtilsLibrary::GetPluginName(FXsollaLoginModule::ModuleName)));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
#endif
}

void UXsollaLoginSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSavedData();

	// Initialize subsystem with project identifiers provided by user
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	Initialize(Settings->ProjectID, Settings->LoginID, Settings->ClientID);

	UE_LOG(LogXsollaLogin, Log, TEXT("%s: XsollaLogin subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaLoginSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaLoginSubsystem::Initialize(const FString& InProjectId, const FString& InLoginId, const FString& InClientId)
{
	ProjectID = InProjectId;
	LoginID = InLoginId;
	ClientID = InClientId;

	// Check token override from Xsolla Launcher
	const FString LauncherLoginJwt = UXsollaLoginLibrary::GetStringCommandLineParam(TEXT("xsolla-login-token"));
	if (!LauncherLoginJwt.IsEmpty())
	{
		UE_LOG(LogXsollaLogin, Warning, TEXT("%s: Xsolla Launcher login token is used"), *VA_FUNC_LINE);
		LoginData.AuthToken.JWT = LauncherLoginJwt;
	}

#if PLATFORM_ANDROID

	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	XsollaMethodCallUtils::CallStaticVoidMethod("com/xsolla/login/XsollaNativeAuth", "xLoginInit",
		"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis,
		XsollaJavaConvertor::GetJavaString(LoginID),
		XsollaJavaConvertor::GetJavaString(ClientID),
		XsollaJavaConvertor::GetJavaString(Settings->FacebookAppId),
		XsollaJavaConvertor::GetJavaString(Settings->GoogleAppId),
		XsollaJavaConvertor::GetJavaString(Settings->WeChatAppId),
		XsollaJavaConvertor::GetJavaString(Settings->QQAppId));

#endif
}

void UXsollaLoginSubsystem::RegisterUser(const FString& Username, const FString& Password, const FString& Email, const FString& State, const FString& Locale, 
	const bool PersonalDataProcessingConsent, const bool ReceiveNewsConsent, const TMap<FString, FString>& AdditionalFields,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	LoginData = FXsollaLoginData();
	LoginData.Username = Username;
	LoginData.Password = Password;

	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetStringField(TEXT("email"), Email);
	RequestDataJson->SetBoolField(TEXT("accept_consent"), PersonalDataProcessingConsent);
	RequestDataJson->SetNumberField(TEXT("promo_email_agreement"), ReceiveNewsConsent ? 1 : 0);
	TSharedPtr<FJsonObject> FieldsDataJson = MakeShareable(new FJsonObject());
	for (auto& FieldKeyValue: AdditionalFields)
	{
		FieldsDataJson->SetStringField(FieldKeyValue.Key, FieldKeyValue.Value);
	}
	RequestDataJson->SetObjectField(TEXT("fields"), FieldsDataJson);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/user"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("locale"), Locale)
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RegisterUser_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ResendAccountConfirmationEmail(const FString& Username, const FString& State, const FString& Locale,
	const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), Username);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/user/resend_confirmation_link"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("locale"), Locale)
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateUser(const FString& Username, const FString& Password,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, const bool bRememberMe)
{
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

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/login/token"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserLogin_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ResetUserPassword(const FString& User, const FString& Locale, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("username"), User);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Endpoint = TEXT("https://login.xsolla.com/api/password/reset/request");
	const FString Url = XsollaUtilsUrlBuilder(Endpoint)
							.AddStringQueryParam(TEXT("projectId"), LoginID)
							.AddStringQueryParam(TEXT("locale"), Locale)
							.AddStringQueryParam(TEXT("login_url"), FGenericPlatformHttp::UrlEncode(Settings->RedirectURI))
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::Default_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(TEXT("https://login.xsolla.com/api/users/me"), EXsollaHttpRequestVerb::VERB_GET, TEXT(""), LoginData.AuthToken.JWT);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::TokenVerify_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetSocialAuthenticationUrl(const FString& ProviderName, const FString& State,
	const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/social/{ProviderName}/login_url"))
							.SetPathParam(TEXT("ProviderName"), ProviderName)
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::LaunchSocialAuthentication(UObject* WorldContextObject, UUserWidget*& BrowserWidget, const bool bRememberMe)
{
	auto MyBrowser = CreateWidget<UUserWidget>(WorldContextObject->GetWorld(), DefaultBrowserWidgetClass);
	MyBrowser->AddToViewport(100000);

	BrowserWidget = MyBrowser;

	// Be sure we've dropped any saved info
	LoginData = FXsollaLoginData();
	LoginData.bRememberMe = bRememberMe;
	SaveData();
}

void UXsollaLoginSubsystem::LaunchNativeSocialAuthentication(const FString& ProviderName, const FOnAuthUpdate& SuccessCallback,
	const FOnAuthCancel& CancelCallback, const FOnAuthError& ErrorCallback, const bool bRememberMe)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	
	NativeSuccessCallback = SuccessCallback;
	NativeErrorCallback = ErrorCallback;
	NativeCancelCallback = CancelCallback;

#if PLATFORM_ANDROID
	UXsollaNativeAuthCallback* nativeCallback = NewObject<UXsollaNativeAuthCallback>();
	nativeCallback->BindSuccessDelegate(SuccessCallback);
	nativeCallback->BindCancelDelegate(CancelCallback);
	nativeCallback->BindErrorDelegate(ErrorCallback);

	XsollaMethodCallUtils::CallStaticVoidMethod("com/xsolla/login/XsollaNativeAuth", "xAuthSocial",
		"(Landroid/app/Activity;Ljava/lang/String;ZJ)V",
		FJavaWrapper::GameActivityThis, 
		XsollaJavaConvertor::GetJavaString(ProviderName), 
		bRememberMe, 
		(jlong)nativeCallback);

#endif

#if PLATFORM_IOS
	OAuth2Params* OAuthParams = [[OAuth2Params alloc] initWithClientId:[Settings->ClientID.GetNSString() intValue]
		state:@"xsollatest"
		scope:@"offline"
		redirectUri:@"app://xsollalogin"];

	JWTGenerationParams* JwtGenerationParams = [[JWTGenerationParams alloc] initWithGrantType:TokenGrantTypeAuthorizationCode
		clientId:[Settings->ClientID.GetNSString() intValue]
		refreshToken:nil
		clientSecret:nil
		redirectUri:@"app://xsollalogin"];

	UIWindow* window = [[UIApplication sharedApplication] keyWindow];
	WebAuthenticationPresentationContextProvider* Context = [[WebAuthenticationPresentationContextProvider alloc] initWithPresentationAnchor:window];

	[[LoginKitObjectiveC shared] authBySocialNetwork:ProviderName.GetNSString()
		oAuth2Params:OAuthParams
		jwtParams:JwtGenerationParams
		presentationContextProvider:Context
		completion:^(AccessTokenInfo* _Nullable tokenInfo, NSError* _Nullable error) {
		if (error != nil)
		{
			NSLog(@"Error code: %ld", error.code);

			if (error.code == NSError.loginKitErrorCodeASCanceledLogin)
			{
				AsyncTask(ENamedThreads::GameThread, [=]() {
					NativeCancelCallback.ExecuteIfBound();
				});
				return;
			}

			AsyncTask(ENamedThreads::GameThread, [=, ErrStr = FString(error.description), ErrCode = FString([@(error.code) stringValue])]() {
				NativeErrorCallback.ExecuteIfBound(ErrCode, ErrStr);
			});
			return;
		}
		
		LoginData.AuthToken.JWT = tokenInfo.accessToken;
		LoginData.AuthToken.RefreshToken = tokenInfo.refreshToken;
		LoginData.AuthToken.ExpiresAt = FDateTime::UtcNow().ToUnixTimestamp() + tokenInfo.expiresIn;

		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeSuccessCallback.ExecuteIfBound(LoginData);
		});
	}];
#endif
}

void UXsollaLoginSubsystem::SetToken(const FString& Token)
{
	LoginData.AuthToken.JWT = Token;
	SaveData();
}

void UXsollaLoginSubsystem::RefreshToken(const FString& RefreshToken, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("client_id"), ClientID);
	RequestDataJson->SetStringField(TEXT("grant_type"), TEXT("refresh_token"));
	RequestDataJson->SetStringField(TEXT("refresh_token"), RefreshToken);
	RequestDataJson->SetStringField(TEXT("redirect_uri"), Settings->RedirectURI);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/token")).Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetContentAsString(UXsollaUtilsLibrary::EncodeFormData(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RefreshToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::ExchangeAuthenticationCodeToToken(const FString& AuthenticationCode, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("client_id"), ClientID);
	RequestDataJson->SetStringField(TEXT("grant_type"), TEXT("authorization_code"));
	RequestDataJson->SetStringField(TEXT("code"), AuthenticationCode);
	RequestDataJson->SetStringField(TEXT("redirect_uri"), Settings->RedirectURI);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/token")).Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetContentAsString(UXsollaUtilsLibrary::EncodeFormData(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RefreshToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthenticateWithSessionTicket(const FString& ProviderName, const FString& SessionTicket, const FString& Code,
	const FString& AppId, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/social/{ProviderName}/cross_auth"))
							.SetPathParam(TEXT("ProviderName"), ProviderName)
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("app_id"), AppId)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.AddStringQueryParam(TEXT("session_ticket"), SessionTicket)
							.AddBoolQueryParam(TEXT("is_redirect"), false, false)
							.AddStringQueryParam(TEXT("code"), Code)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SessionTicket_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetUserAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
	const FOnUserAttributesUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));
	if (!UserId.IsEmpty())
	{
		RequestDataJson->SetStringField(TEXT("user_id"), UserId);
	}

	TArray<TSharedPtr<FJsonValue>> KeysJsonArray;
	for (auto& Key : AttributeKeys)
	{
		KeysJsonArray.Push(MakeShareable(new FJsonValueString(Key)));
	}

	RequestDataJson->SetArrayField(TEXT("keys"), KeysJsonArray);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/attributes/users/me/get")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::GetUserAttributes_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});
	
	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetUserReadOnlyAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
	const FOnUserAttributesUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));
	if (!UserId.IsEmpty())
	{
		RequestDataJson->SetStringField(TEXT("user_id"), UserId);
	}

	TArray<TSharedPtr<FJsonValue>> KeysJsonArray;
	for (auto& Key : AttributeKeys)
	{
		KeysJsonArray.Push(MakeShareable(new FJsonValueString(Key)));
	}

	RequestDataJson->SetArrayField(TEXT("keys"), KeysJsonArray);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/attributes/users/me/get_read_only")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::GetReadOnlyUserAttributes_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::ModifyUserAttributes(const FString& AuthToken, const TArray<FXsollaUserAttribute>& AttributesToModify,
	const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());

	TArray<TSharedPtr<FJsonValue>> AttributesJsonArray;
	for (auto& Attribute : AttributesToModify)
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

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/attributes/users/me/update")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::DefaultWithHandlerWrapper_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove,
	const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetNumberField(TEXT("publisher_project_id"), FCString::Atoi(*ProjectID));
	SetStringArrayField(RequestDataJson, TEXT("removing_keys"), AttributesToRemove);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/attributes/users/me/update")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::DefaultWithHandlerWrapper_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::CheckUserAge(const FString& DateOfBirth, const FOnCheckUserAgeSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());

	RequestDataJson->SetStringField(TEXT("dob"), *DateOfBirth);
	RequestDataJson->SetStringField(TEXT("project_id"), *LoginID);

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/age/check")).Build();

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::CheckUserAge_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::LinkEmailAndPassword(const FString& AuthToken, const FString& Email, const FString& Password, const bool ReceiveNewsConsent, const FString& Username,
	const FOnLinkEmailAndPasswordSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request body
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("email"), Email);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetNumberField(TEXT("promo_email_agreement"), ReceiveNewsConsent ? 1 : 0);
	RequestDataJson->SetStringField(TEXT("username"), Username);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/link_email_password"))
							.AddStringQueryParam(TEXT("login_url"), FGenericPlatformHttp::UrlEncode(Settings->RedirectURI))
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::LinkEmailAndPassword_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::LinkDeviceToAccount(const FString& AuthToken, const FString& PlatformName, const FString& DeviceName, const FString& DeviceId,
	const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("device"), DeviceName);
	RequestDataJson->SetStringField(TEXT("device_id"), DeviceId);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/devices/{PlatformName}"))
							.SetPathParam(TEXT("PlatformName"), PlatformName.ToLower())
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::DefaultWithHandlerWrapper_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::UnlinkDeviceFromAccount(const FString& AuthToken, const int64 DeviceId, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/devices/{DeviceId}"))
							.SetPathParam(TEXT("DeviceId"), DeviceId)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_DELETE, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::DefaultWithHandlerWrapper_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::AuthenticateViaDeviceId(const FString& DeviceName, const FString& DeviceId, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const FString& PlatformName = UGameplayStatics::GetPlatformName();
	if (PlatformName != "Android" && PlatformName != "IOS")
	{
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: User authentication should be handled via android mobile or ios mobile"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(TEXT("Authentication failed"), TEXT("User authentication should be handled via Android mobile or IOS mobile"));
		return;
	}

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("device"), DeviceName);
	RequestDataJson->SetStringField(TEXT("device_id"), DeviceId);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/login/device/{PlatformName}"))
							.SetPathParam(TEXT("PlatformName"), PlatformName.ToLower())
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::DeviceId_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetwork(
	const FString& AuthToken, const FString& AuthTokenSecret, const FString& OpenId,
	const FString& ProviderName, const FString& State,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/social/{ProviderName}/login_with_token"))
							.SetPathParam(TEXT("ProviderName"), ProviderName)
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.Build();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("access_token"), *AuthToken);
	if (ProviderName.Compare(TEXT("twitter")))
	{
		RequestDataJson->SetStringField(TEXT("access_token_secret"), *AuthTokenSecret);
	}
	if (ProviderName.Compare(TEXT("wechat")))
	{
		RequestDataJson->SetStringField(TEXT("openid"), *OpenId);
	}

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetwork_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::StartAuthByPhoneNumber(const FString& PhoneNumber, const FString& State,
	const FOnStartAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("phone_number"), PhoneNumber);

	if (Settings->SendPasswordlessAuthURL)
	{
		RequestDataJson->SetBoolField(TEXT("send_link"), true);
		RequestDataJson->SetStringField(TEXT("link_url"), Settings->PasswordlessAuthURL);
	}

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/login/phone/request"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::StartAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::CompleteAuthByPhoneNumber(const FString& Code, const FString& OperationId, const FString& PhoneNumber,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("code"), Code);
	RequestDataJson->SetStringField(TEXT("operation_id"), OperationId);
	RequestDataJson->SetStringField(TEXT("phone_number"), PhoneNumber);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/login/phone/confirm"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::CompleteAuthByPhoneNumber_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::StartAuthByEmail(const FString& Email, const FString& State,
	const FOnStartAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("email"), Email);

	if (Settings->SendPasswordlessAuthURL)
	{
		RequestDataJson->SetBoolField(TEXT("send_link"), true);
		RequestDataJson->SetStringField(TEXT("link_url"), Settings->PasswordlessAuthURL);
	}

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/login/email/request"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.AddStringQueryParam(TEXT("response_type"), TEXT("code"))
							.AddStringQueryParam(TEXT("redirect_uri"), Settings->RedirectURI)
							.AddStringQueryParam(TEXT("state"), State)
							.AddStringQueryParam(TEXT("scope"), TEXT("offline"))
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::StartAuth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::CompleteAuthByEmail(const FString& Code, const FString& OperationId, const FString& Email,
	const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("code"), Code);
	RequestDataJson->SetStringField(TEXT("operation_id"), OperationId);
	RequestDataJson->SetStringField(TEXT("email"), Email);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/login/email/confirm"))
							.AddStringQueryParam(TEXT("client_id"), ClientID)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::CompleteAuthByEmail_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetAuthConfirmationCode(const FString& UserId, const FString& OperationId,
	const FOnAuthCodeSuccess& SuccessCallback, const FOnAuthCodeTimeout& TimeoutCallback, const FOnAuthError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/otc/code"))
							.AddStringQueryParam(TEXT("projectId"), LoginID)
							.AddStringQueryParam(TEXT("login"), FGenericPlatformHttp::UrlEncode(UserId))
							.AddStringQueryParam(TEXT("operation_id"), OperationId)
							.Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::GetAuthConfirmationCode_HttpRequestComplete, SuccessCallback, TimeoutCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::GetUserDetails(const FString& AuthToken, const FOnUserDetailsUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me")).Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserDetails_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::ModifyUserDetails(const FString& AuthToken, const FString& Birthday, const FString& FirstName, const FString& LastName,
	const FString& Gender, const FString& Nickname, const FOnUserDetailsUpdate& SuccessCallback, const FOnError& ErrorCallback)
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

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(TEXT("https://login.xsolla.com/api/users/me"), EXsollaHttpRequestVerb::VERB_PATCH, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserDetails_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetUserEmail(const FString& AuthToken, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/email")).Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserEmail_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetUserPhoneNumber(const FString& AuthToken, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/phone")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserPhoneNumber_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::ModifyUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber,
	const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("phone_number"), PhoneNumber);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/phone")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::ModifyPhoneNumber_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::RemoveUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber,
	const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/phone/{phoneNumber}"))
							.SetPathParam(TEXT("phoneNumber"), PhoneNumber)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_DELETE, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::RemovePhoneNumber_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::ModifyUserProfilePicture(const FString& AuthToken, const UTexture2D* const Picture,
	const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	if (!IsValid(Picture))
	{
		ErrorCallback.Execute(-1, -1, "Picture is invalid.");
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
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/picture")).Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, Boundary, UploadContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, TEXT(""), Token);
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary =" + Boundary));
		HttpRequest->SetContent(UploadContent);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserProfilePicture_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::RemoveProfilePicture(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/picture")).Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_DELETE, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserProfilePictureRemove_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetFriends(const FString& AuthToken, const EXsollaFriendsType Type, const EXsollaUsersSortCriteria SortBy, const EXsollaUsersSortOrder SortOrder,
	const FOnUserFriendsUpdate& SuccessCallback, const FOnError& ErrorCallback, const FString& After, const int Limit)
{
	const FString FriendType = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaFriendsType", Type);
	const FString SortByCriteria = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaUsersSortCriteria", SortBy);
	const FString SortOrderCriteria = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaUsersSortOrder", SortOrder);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/relationships"))
							.AddStringQueryParam(TEXT("type"), FriendType)
							.AddStringQueryParam(TEXT("sort_by"), SortByCriteria)
							.AddStringQueryParam(TEXT("sort_order"), SortOrderCriteria)
							.AddStringQueryParam(TEXT("after"), After)
							.AddNumberQueryParam(TEXT("limit"), Limit)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserFriends_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::ModifyFriends(const FString& AuthToken, const EXsollaFriendAction Action, const FString& UserID,
	const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("user"), UserID);
	RequestDataJson->SetStringField(TEXT("action"), UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaFriendAction", Action));

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/relationships")).Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, PostContent, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, PostContent, Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::DefaultWithHandlerWrapper_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetSocialAuthLinks(const FString& AuthToken, const FString& Locale, const FOnSocialAuthLinksUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/login_urls"))
							.AddStringQueryParam(TEXT("locale"), Locale)
							.Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAuthLinks_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetSocialFriends(const FString& AuthToken, const FString& Platform,
	const FOnUserSocialFriendsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Offset, const int Limit, const bool FromThisGame)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/social_friends"))
							.AddNumberQueryParam(TEXT("limit"), Limit)
							.AddNumberQueryParam(TEXT("offset"), Offset)
							.AddBoolQueryParam(TEXT("with_xl_uid"), FromThisGame, false)
							.AddStringQueryParam(TEXT("platform"), Platform)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialFriends_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetUsersFriends(const FString& AuthToken, const FString& Platform, const FOnCodeReceived& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/social_friends/update"))
							.AddStringQueryParam(TEXT("platform"), Platform)
							.Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::GetUsersFriends_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetUserProfile(const FString& AuthToken, const FString& UserID, const FOnUserProfileReceived& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/{UserID}/public"))
							.SetPathParam("UserID", UserID)
							.Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserProfile_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetUsersDevices(const FString& AuthToken, const FOnUserDevicesUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/devices")).Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::GetUsersDevices_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::SearchUsersByNickname(const FString& AuthToken, const FString& Nickname,
	const FOnUserSearchUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Offset, const int Limit)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/search/by_nickname"))
							.AddStringQueryParam(TEXT("nickname"), FGenericPlatformHttp::UrlEncode(Nickname))
							.AddNumberQueryParam(TEXT("limit"), Limit)
							.AddNumberQueryParam(TEXT("offset"), Offset)
							.Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::UserSearch_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::LinkSocialNetworkToUserAccount(const FString& AuthToken, const FString& ProviderName,
	const FOnSocialAccountLinkingHtmlReceived& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/social_providers/{ProviderName}/login_url"))
							.SetPathParam(TEXT("ProviderName"), ProviderName)
							.AddStringQueryParam(TEXT("login_url"), FGenericPlatformHttp::UrlEncode(Settings->RedirectURI))
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::SocialAccountLinking_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::GetLinkedSocialNetworks(const FString& AuthToken, const FOnLinkedSocialNetworksUpdate& SuccessCallback, const FOnError& ErrorCallback)
{
	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/users/me/social_providers")).Build();
	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::LinkedSocialNetworks_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::LogoutUser(const FString& AuthToken, const EXsollaSessionType Sessions,
	const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback)
{
	const FString SessionsString = UXsollaUtilsLibrary::GetEnumValueAsString("EXsollaSessionType", Sessions);
	
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/logout"))
							.AddStringQueryParam(TEXT("sessions"), SessionsString)
							.Build();

	FOnTokenUpdate SuccessTokenUpdate;
	SuccessTokenUpdate.BindLambda([&, Url, SuccessCallback, ErrorCallback, SuccessTokenUpdate](const FString& Token, bool bRepeatOnError)
	{
		const auto ErrorHandlersWrapper = FErrorHandlersWrapper(bRepeatOnError, SuccessTokenUpdate, ErrorCallback);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_GET, TEXT(""), Token);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::LogoutUser_HttpRequestComplete, SuccessCallback, ErrorHandlersWrapper);
		HttpRequest->ProcessRequest();
	});

	SuccessTokenUpdate.ExecuteIfBound(AuthToken, true);
}

void UXsollaLoginSubsystem::Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestOAuthError(OutError, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::DefaultWithHandlerWrapper_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleOAuthTokenRequest(HttpRequest, HttpResponse, bSucceeded, ErrorCallback, SuccessCallback);
}

void UXsollaLoginSubsystem::TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		LoginData.AuthToken.bIsVerified = true;
		SaveData();
		SuccessCallback.ExecuteIfBound(LoginData);
	}
	else
	{
		HandleRequestOAuthError(OutError, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::SocialAuthUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnSocialUrlReceived SuccessCallback, FOnAuthError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString SocialUrlFieldName = TEXT("url");
		if (JsonObject->HasTypedField<EJson::String>(SocialUrlFieldName))
		{
			const FString SocialnUrl = JsonObject.Get()->GetStringField(SocialUrlFieldName);
			SuccessCallback.ExecuteIfBound(SocialnUrl);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *SocialUrlFieldName);
	}

	HandleRequestOAuthError(OutError, ErrorCallback);
}

void UXsollaLoginSubsystem::GetUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserAttributesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	TArray<FXsollaUserAttribute> userAttributesData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsArray(HttpRequest, HttpResponse, bSucceeded, &userAttributesData, OutError))
	{
		SuccessCallback.ExecuteIfBound(userAttributesData);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::GetReadOnlyUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserAttributesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	TArray<FXsollaUserAttribute> userReadOnlyAttributesData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsArray(HttpRequest, HttpResponse, bSucceeded, &userReadOnlyAttributesData, OutError))
	{
		SuccessCallback.ExecuteIfBound(userReadOnlyAttributesData);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::CheckUserAge_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnCheckUserAgeSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	XsollaHttpRequestError OutError;
	FXsollaCheckUserAgeResult CheckUserAgeResult;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaCheckUserAgeResult::StaticStruct(), &CheckUserAgeResult, OutError))
	{
		SuccessCallback.ExecuteIfBound(CheckUserAgeResult);
	}
	else
	{
		HandleRequestOAuthError(OutError, ErrorCallback);
	}
}

void UXsollaLoginSubsystem::DeviceId_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleUrlWithCodeRequest(HttpRequest, HttpResponse, bSucceeded, SuccessCallback, ErrorCallback);
}

void UXsollaLoginSubsystem::RefreshToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleOAuthTokenRequest(HttpRequest, HttpResponse, bSucceeded, ErrorCallback, SuccessCallback);
}

void UXsollaLoginSubsystem::InnerRefreshToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnLoginDataUpdate SuccessCallback, FOnLoginDataError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString AccessTokenFieldName = TEXT("access_token");
		if (JsonObject->HasTypedField<EJson::String>(AccessTokenFieldName))
		{
			LoginData.AuthToken.JWT = JsonObject->GetStringField(AccessTokenFieldName);
			LoginData.AuthToken.RefreshToken = JsonObject->GetStringField(TEXT("refresh_token"));
			LoginData.AuthToken.ExpiresAt = FDateTime::UtcNow().ToUnixTimestamp() + JsonObject->GetNumberField(TEXT("expires_in"));

			SaveData();

			SuccessCallback.ExecuteIfBound(LoginData);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *AccessTokenFieldName);
	}

	ErrorCallback.ExecuteIfBound(OutError.statusCode, OutError.errorCode, OutError.description);
}

void UXsollaLoginSubsystem::SessionTicket_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleUrlWithCodeRequest(HttpRequest, HttpResponse, bSucceeded, SuccessCallback, ErrorCallback);
}

void UXsollaLoginSubsystem::AuthViaAccessTokenOfSocialNetwork_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleUrlWithCodeRequest(HttpRequest, HttpResponse, bSucceeded, SuccessCallback, ErrorCallback);
}

void UXsollaLoginSubsystem::StartAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnStartAuthSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString OperationIdFieldName = TEXT("operation_id");
		if (JsonObject->HasTypedField<EJson::String>(OperationIdFieldName))
		{
			const FString operationId = JsonObject->GetStringField(OperationIdFieldName);
			SuccessCallback.ExecuteIfBound(operationId);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *OperationIdFieldName);
	}

	HandleRequestOAuthError(OutError, ErrorCallback);
}

void UXsollaLoginSubsystem::CompleteAuthByPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleUrlWithCodeRequest(HttpRequest, HttpResponse, bSucceeded, SuccessCallback, ErrorCallback);
}

void UXsollaLoginSubsystem::CompleteAuthByEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	HandleUrlWithCodeRequest(HttpRequest, HttpResponse, bSucceeded, SuccessCallback, ErrorCallback);
}

void UXsollaLoginSubsystem::GetAuthConfirmationCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthCodeSuccess SuccessCallback, FOnAuthCodeTimeout TimeoutCallback, FOnAuthError ErrorCallback)
{
	const FString OperationId = UXsollaUtilsLibrary::GetUrlParameter(HttpRequest->GetURL(), TEXT("operation_id"));

	if (HttpResponse->GetResponseCode() == EHttpResponseCodes::Type::RequestTimeout)
	{
		TimeoutCallback.ExecuteIfBound(OperationId);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString CodeFieldName = TEXT("code");
		if (JsonObject->HasTypedField<EJson::String>(CodeFieldName))
		{
			const FString Code = JsonObject->GetStringField(CodeFieldName);
			SuccessCallback.ExecuteIfBound(Code, OperationId);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *CodeFieldName);
	}

	HandleRequestOAuthError(OutError, ErrorCallback);
}

void UXsollaLoginSubsystem::UserDetails_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserDetailsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FXsollaUserDetails receivedUserDetails;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaUserDetails::StaticStruct(), &receivedUserDetails, OutError))
	{
		SuccessCallback.ExecuteIfBound(receivedUserDetails);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::UserEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString EmailFieldName = TEXT("current_email");
		if (JsonObject->HasTypedField<EJson::String>(EmailFieldName))
		{
			auto Email = JsonObject->GetStringField(EmailFieldName);
			SuccessCallback.ExecuteIfBound(Email);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *EmailFieldName);
	}

	HandleRequestError(OutError, ErrorHandlersWrapper);
}

void UXsollaLoginSubsystem::UserPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString PhoneFieldName = TEXT("phone_number");
		if (JsonObject->HasTypedField<EJson::String>(PhoneFieldName))
		{
			auto Phone = JsonObject->GetStringField(PhoneFieldName);
			SuccessCallback.ExecuteIfBound(Phone);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *PhoneFieldName);
	}

	HandleRequestError(OutError, ErrorHandlersWrapper);
}

void UXsollaLoginSubsystem::ModifyPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		const TArray<uint8> RequestContent = HttpRequest->GetContent();
		FUTF8ToTCHAR Converter((ANSICHAR*)RequestContent.GetData(), RequestContent.Num());
		FString RequestContentStr = FString(Converter.Length(), Converter.Get());

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*RequestContentStr);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			static const FString PhoneFieldName = TEXT("phone_number");
			if (JsonObject->HasTypedField<EJson::String>(PhoneFieldName))
			{
				auto Phone = JsonObject->GetStringField(PhoneFieldName);
				SuccessCallback.ExecuteIfBound(Phone);
				return;
			}

			OutError.description = FString::Printf(TEXT("No field '%s' found"), *PhoneFieldName);
		}
	}

	HandleRequestError(OutError, ErrorHandlersWrapper);
}

void UXsollaLoginSubsystem::RemovePhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::UserProfilePicture_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString PictureFieldName = TEXT("picture");
		if (JsonObject->HasTypedField<EJson::String>(PictureFieldName))
		{
			auto Picture = JsonObject->GetStringField(PictureFieldName);
			SuccessCallback.ExecuteIfBound(Picture);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *PictureFieldName);
	}

	HandleRequestError(OutError, ErrorHandlersWrapper);
}

void UXsollaLoginSubsystem::UserProfilePictureRemove_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::UserFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserFriendsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FXsollaFriendsData receivedUserFriendsData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaFriendsData::StaticStruct(), &receivedUserFriendsData, OutError))
	{
		const FString Type = UXsollaUtilsLibrary::GetUrlParameter(HttpRequest->GetURL(), TEXT("type"));

		SuccessCallback.ExecuteIfBound(receivedUserFriendsData, UXsollaUtilsLibrary::GetEnumValueFromString<EXsollaFriendsType>("EXsollaFriendsType", Type));
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::SocialAuthLinks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnSocialAuthLinksUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	TArray<FXsollaSocialAuthLink> socialAuthLinksData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsArray(HttpRequest, HttpResponse, bSucceeded, &socialAuthLinksData, OutError))
	{
		SuccessCallback.ExecuteIfBound(socialAuthLinksData);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::SocialFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserSocialFriendsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FXsollaSocialFriendsData receivedUserSocialFriendsData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaSocialFriendsData::StaticStruct(), &receivedUserSocialFriendsData, OutError))
	{
		SuccessCallback.ExecuteIfBound(receivedUserSocialFriendsData);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::GetUsersFriends_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnCodeReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound(TEXT("204"));
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::UserProfile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserProfileReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FXsollaPublicProfile receivedUserProfile;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaPublicProfile::StaticStruct(), &receivedUserProfile, OutError))
	{
		SuccessCallback.ExecuteIfBound(receivedUserProfile);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::UserSearch_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserSearchUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	FXsollaUserSearchResult searchResult;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsStruct(HttpRequest, HttpResponse, bSucceeded, FXsollaUserSearchResult::StaticStruct(), &searchResult, OutError))
	{
		SuccessCallback.ExecuteIfBound(searchResult);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::SocialAccountLinking_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnSocialAccountLinkingHtmlReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString SocialUrlFieldName = TEXT("url");
		if (JsonObject->HasTypedField<EJson::String>(SocialUrlFieldName))
		{
			FString SocialnUrl = JsonObject.Get()->GetStringField(SocialUrlFieldName);
			SuccessCallback.ExecuteIfBound(SocialnUrl);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *SocialUrlFieldName);
	}

	HandleRequestError(OutError, ErrorHandlersWrapper);
}

void UXsollaLoginSubsystem::LinkedSocialNetworks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnLinkedSocialNetworksUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	TArray<FXsollaLinkedSocialNetworkData> socialNetworks;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsArray(HttpRequest, HttpResponse, bSucceeded, &socialNetworks, OutError))
	{
		SuccessCallback.ExecuteIfBound(socialNetworks);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::GetUsersDevices_HttpRequestComplete(const FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnUserDevicesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	TArray<FXsollaUserDevice> userDevicesData;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsArray(HttpRequest, HttpResponse, bSucceeded, &userDevicesData, OutError))
	{
		SuccessCallback.ExecuteIfBound(userDevicesData);
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::LinkEmailAndPassword_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnLinkEmailAndPasswordSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString ConfirmationRequiredFieldName = TEXT("email_confirmation_required");
		if (JsonObject->HasTypedField<EJson::Boolean>(ConfirmationRequiredFieldName))
		{
			const bool bNeedToConfirmEmail = JsonObject->GetBoolField(ConfirmationRequiredFieldName);
			SuccessCallback.ExecuteIfBound(bNeedToConfirmEmail);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *ConfirmationRequiredFieldName);
	}

	HandleRequestError(OutError, ErrorHandlersWrapper);
}

void UXsollaLoginSubsystem::RegisterUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	if (HttpResponse->GetResponseCode() == EHttpResponseCodes::Type::Ok)
	{
		
		HandleUrlWithCodeRequest(HttpRequest, HttpResponse, bSucceeded, SuccessCallback, ErrorCallback);
		return;
	}

	if (HttpResponse->GetResponseCode() == EHttpResponseCodes::Type::NoContent)
	{
		SuccessCallback.ExecuteIfBound(LoginData);
		return;
	}

	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		OutError.description = TEXT("Response code is successfull, but outcome is unexpected");
	}

	HandleRequestOAuthError(OutError, ErrorCallback);
}

void UXsollaLoginSubsystem::LogoutUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	const bool bSucceeded, FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	XsollaHttpRequestError OutError;
	
	if (XsollaUtilsHttpRequestHelper::ParseResponse(HttpRequest, HttpResponse, bSucceeded, OutError))
	{
		SuccessCallback.ExecuteIfBound();
	}
	else
	{
		HandleRequestError(OutError, ErrorHandlersWrapper);
	}
}

void UXsollaLoginSubsystem::HandleOAuthTokenRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthError& ErrorCallback, FOnAuthUpdate& SuccessCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString AccessTokenFieldName = TEXT("access_token");
		if (JsonObject->HasTypedField<EJson::String>(AccessTokenFieldName))
		{
			LoginData.AuthToken.JWT = JsonObject->GetStringField(AccessTokenFieldName);
			LoginData.AuthToken.RefreshToken = JsonObject->GetStringField(TEXT("refresh_token"));
			LoginData.AuthToken.ExpiresAt = FDateTime::UtcNow().ToUnixTimestamp() + JsonObject->GetNumberField(TEXT("expires_in"));

			SaveData();

			SuccessCallback.ExecuteIfBound(LoginData);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *AccessTokenFieldName);
	}

	HandleRequestOAuthError(OutError, ErrorCallback);
}

void UXsollaLoginSubsystem::HandleUrlWithCodeRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject;
	XsollaHttpRequestError OutError;

	if (XsollaUtilsHttpRequestHelper::ParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, JsonObject, OutError))
	{
		static const FString LoginUrlFieldName = TEXT("login_url");
		if (JsonObject->HasTypedField<EJson::String>(LoginUrlFieldName))
		{
			const FString LoginUrl = JsonObject.Get()->GetStringField(LoginUrlFieldName);
			const FString Code = UXsollaUtilsLibrary::GetUrlParameter(LoginUrl, TEXT("code"));

			ExchangeAuthenticationCodeToToken(Code, SuccessCallback, ErrorCallback);
			return;
		}

		OutError.description = FString::Printf(TEXT("No field '%s' found"), *LoginUrlFieldName);
	}

	HandleRequestOAuthError(OutError, ErrorCallback);
}

void UXsollaLoginSubsystem::HandleRequestOAuthError(XsollaHttpRequestError ErrorData, FOnAuthError ErrorCallback)
{
	UE_LOG(LogXsollaLogin, Error, TEXT("%s: request failed - Error code: %s, Error message: %s"), *VA_FUNC_LINE, *ErrorData.code, *ErrorData.description);
	ErrorCallback.ExecuteIfBound(ErrorData.code, ErrorData.description);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaLoginSubsystem::CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb, const FString& Content, const FString& AuthToken)
{
	return XsollaUtilsHttpRequestHelper::CreateHttpRequest(Url, Verb, AuthToken, Content, TEXT("LOGIN"), XSOLLA_LOGIN_VERSION);
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

FXsollaLoginData UXsollaLoginSubsystem::GetLoginData() const
{
	return LoginData;
}

void UXsollaLoginSubsystem::SetLoginData(const FXsollaLoginData& Data, const bool ClearCache)
{
	// Refresh saved data in memory
	LoginData = Data;

	if (ClearCache)
	{
		// Drop saved data in cache
		UXsollaLoginSave::Save(LoginData);
	}
}

void UXsollaLoginSubsystem::UpdateAuthTokenData(const FString& AccessToken, int ExpiresIn, const FString& RefreshToken, bool bRememberMe)
{
	LoginData.AuthToken.JWT = AccessToken;
	LoginData.AuthToken.ExpiresAt = ExpiresIn;
	LoginData.AuthToken.RefreshToken = RefreshToken;
	LoginData.bRememberMe = bRememberMe;
}

void UXsollaLoginSubsystem::DropLoginData(const bool ClearCache)
{
	// Drop saved data in memory
	LoginData = FXsollaLoginData();

	if (ClearCache)
	{
		// Drop saved data in cache
		UXsollaLoginSave::Save(LoginData);
	}
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

void UXsollaLoginSubsystem::InnerRefreshToken(const FString& RefreshToken, const FOnLoginDataUpdate& SuccessCallback, const FOnLoginDataError& ErrorCallback)
{
	const UXsollaProjectSettings* Settings = FXsollaSettingsModule::Get().GetSettings();

	// Prepare request payload
	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("client_id"), ClientID);
	RequestDataJson->SetStringField(TEXT("grant_type"), TEXT("refresh_token"));
	RequestDataJson->SetStringField(TEXT("refresh_token"), RefreshToken);
	RequestDataJson->SetStringField(TEXT("redirect_uri"), Settings->RedirectURI);

	FString PostContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	// Generate endpoint url
	const FString Url = XsollaUtilsUrlBuilder(TEXT("https://login.xsolla.com/api/oauth2/token")).Build();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(Url, EXsollaHttpRequestVerb::VERB_POST);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	HttpRequest->SetContentAsString(UXsollaUtilsLibrary::EncodeFormData(RequestDataJson));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaLoginSubsystem::InnerRefreshToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaLoginSubsystem::HandleRequestError(const XsollaHttpRequestError& ErrorData, FErrorHandlersWrapper ErrorHandlersWrapper)
{
	if (ErrorData.statusCode == 401 || ErrorData.statusCode == 403) // token time expired
	{
		FOnLoginDataUpdate SuccessRefreshCallback;
		SuccessRefreshCallback.BindLambda([ErrorHandlersWrapper](const FXsollaLoginData& InLoginData)
		{
			if (ErrorHandlersWrapper.bNeedRepeatRequest)
			{
				ErrorHandlersWrapper.TokenUpdateCallback.ExecuteIfBound(InLoginData.AuthToken.JWT, false);
			}
		});
		FOnLoginDataError ErrorRefreshCallback;
		ErrorRefreshCallback.BindLambda([ErrorHandlersWrapper](int32 StatusCode, int32 ErrorCode, const FString& Description)
		{
			ErrorHandlersWrapper.ErrorCallback.ExecuteIfBound(StatusCode, ErrorCode, Description);
		});

		InnerRefreshToken(LoginData.AuthToken.RefreshToken, SuccessRefreshCallback, ErrorRefreshCallback);
	}
	else
	{
		auto errorMessage = ErrorData.errorMessage.IsEmpty() ? ErrorData.description : ErrorData.errorMessage;
		UE_LOG(LogXsollaLogin, Error, TEXT("%s: request failed - Status code: %d, Error code: %d, Error message: %s"), *VA_FUNC_LINE, ErrorData.statusCode, ErrorData.errorCode, *errorMessage);
		ErrorHandlersWrapper.ErrorCallback.ExecuteIfBound(ErrorData.statusCode, ErrorData.errorCode, errorMessage);
	}
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
