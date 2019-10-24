// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginTypes.h"

#include "Blueprint/UserWidget.h"
#include "Http.h"

#include "XsollaLoginController.generated.h"

/** Common callback for operations without any user-friendly messages from server on success */
DECLARE_DYNAMIC_DELEGATE(FOnRequestSuccess);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuthUpdate, const FXsollaLoginData&, LoginData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSocialUrlReceived, const FString&, Url);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAuthError, const FString&, Code, const FString&, Description);

UCLASS()
class XSOLLALOGIN_API UXsollaLoginController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Initialize controller with provided project id (use to override project settings) */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void Initialize(const FString& InLoginProjectId);

	/**
	 * Adds a new user to the database. The user will receive an account confirmation message to the specified email.
	 *
	 * @param Username Username. Required.
	 * @param Password Password. Required.
	 * @param Email Email. Required.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RegistrateUser(const FString& Username, const FString& Password, const FString& Email, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/**
	 * Authenticates the user by the username and password specified.
	 *
	 * @param Username Username. Required.
	 * @param Password Password. Required.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticateUser(const FString& Username, const FString& Password, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, bool bRememberMe = false);

	/**
	 * Resets the user's password.
	 *
	 * @param Username Username. Required.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResetUserPassword(const FString& Username, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Internal request for token validation (called with each auth update automatically) */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Get URL for authentication via specified social network
	 *
	 * @param ProviderName Name of social network. Provider must be enabled for the project. Required.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSocialAuthenticationUrl(const FString& ProviderName, const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Open social authentication URL in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void LaunchSocialAuthentication(const FString& SocialAuthenticationUrl, UUserWidget*& BrowserWidget);

	/** Set new value of token (used when token obtained via social network authentication etc.) */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetToken(const FString& token);

protected:
	void Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SocialAuthUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnSocialUrlReceived SuccessCallback, FOnAuthError ErrorCallback);

	/** Return true if error is happened */
	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError ErrorCallback);

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url, const FString& Content);

	/** Cached Xsolla Login project id */
	FString LoginProjectId;

public:
	/** Get user login state data */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Login")
	FXsollaLoginData GetLoginData();

	/** Drop cache and cleanup login data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void DropLoginData();

	/** Load save game and extract data */
	void LoadSavedData();

	/** Save cached data or reset one if RememberMe is false */
	void SaveData();

	/** Get pending social authentication url to be opened in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetPendingSocialAuthenticationUrl() const;

protected:
	/** Keeps state of user login */
	FXsollaLoginData LoginData;

	/** Social authentication url to be opened in browser */
	FString PendingSocialAuthenticationUrl;

protected:
	static const FString RegistrationEndpoint;
	static const FString LoginEndpoint;
	static const FString LoginSocialEndpoint;
	static const FString ResetPasswordEndpoint;

	static const FString ProxyRegistrationEndpoint;
	static const FString ProxyLoginEndpoint;
	static const FString ProxyResetPasswordEndpoint;

	static const FString ValidateTokenEndpoint;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;
};
