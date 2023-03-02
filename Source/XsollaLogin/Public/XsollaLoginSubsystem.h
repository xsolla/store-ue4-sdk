// Copyright 2023 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaLoginTypes.h"

#include "XsollaUtilsDataModel.h"
#include "XsollaUtilsHttpRequestHelper.h"

#include "Blueprint/UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "XsollaUtilsDataModel.h"

#include "XsollaLoginSubsystem.generated.h"

class FJsonObject;
class UXsollaLoginBrowserWrapper;

/** Common callback for operations without any user-friendly messages from the server in case of success. */
DECLARE_DYNAMIC_DELEGATE(FOnRequestSuccess);
DECLARE_DELEGATE_ThreeParams(FOnLoginDataError, int32, int32, const FString&);
DECLARE_DELEGATE_OneParam(FOnLoginDataUpdate, const FXsollaLoginData&);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuthUpdate, const FXsollaLoginData&, LoginData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSocialUrlReceived, const FString&, Url);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSocialAccountLinkingHtmlReceived, const FString&, Content);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAuthError, const FString&, Code, const FString&, Description);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCodeReceived, const FString&, Code);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnUserFriendsUpdate, const FXsollaFriendsData&, FriendsData, EXsollaFriendsType, type);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserSocialFriendsUpdate, const FXsollaSocialFriendsData&, SocialFriendsData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserProfileReceived, const FXsollaPublicProfile&, UserProfile);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserSearchUpdate, const FXsollaUserSearchResult&, SearchResult);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCheckUserAgeSuccess, const FXsollaCheckUserAgeResult&, CheckUserAgeResult);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAccessTokenLoginSuccess, FString, AccessToken);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLinkEmailAndPasswordSuccess, bool, bNeedToConfirmEmail);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStartAuthSuccess, FString, OperationId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAuthCodeSuccess, const FString&, Code, const FString&, OperationId);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuthCodeTimeout, const FString&, OperationId);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserAttributesUpdate, const TArray<FXsollaUserAttribute>&, UserAttributes);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserDetailsUpdate, const FXsollaUserDetails&, UserDetails);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserDetailsParamUpdate, const FString&, Param);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSocialAuthLinksUpdate, const TArray<FXsollaSocialAuthLink>&, Links);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLinkedSocialNetworksUpdate, const TArray<FXsollaLinkedSocialNetworkData>&, LinkedSocialNetworks);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserDevicesUpdate, const TArray<FXsollaUserDevice>&, UserDevices);
DECLARE_DYNAMIC_DELEGATE(FOnAuthCancel);

UCLASS()
class XSOLLALOGIN_API UXsollaLoginSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UXsollaLoginSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/** Initializes controller with provided Project ID and Login ID (use to override project settings).
	 *
	 * @param InProjectId New Project ID value from Publisher Account. It can be found in Publisher Account next to the name of your project.
	 * @param InLoginId New Login ID value from Publisher Account. To get it, open Publisher Account, go to the **Login > Dashboard** section, and click **Copy ID** beside the name of the Login project.
	 * @param InClientId New Client ID value from Publisher Account. It can be found in Publisher Account in the **Login > your Login project > Security > OAuth 2.0 section**.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void Initialize(const FString& InProjectId, const FString& InLoginId, const FString& InClientId);

	/** Creates a new user account in the application and sends a sign-up confirmation email to the specified email address. To complete registration, the user must follow the link from the email.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/classic-auth/).
	 *
	 * @param Username Username.
	 * @param Password Password.
	 * @param Email Email address.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param Locale Defines localization of the email the user receives.<br>
	 * The following languages are supported: Arabic (`ar_AE`), Bulgarian (`bg_BG`), Czech (`cz_CZ`), German (`de_DE`), Spanish (`es_ES`), French (`fr_FR`), Hebrew (`he_IL`), Italian (`it_IT`), Japanese (`ja_JP`), Korean (`ko_KR`), Polish (`pl_PL`), Portuguese (`pt_BR`), Romanian (`ro_RO`), Russian (`ru_RU`), Thai (`th_TH`), Turkish (`tr_TR`), Vietnamese (`vi_VN`), Chinese Simplified (`zh_CN`), Chinese Traditional (`zh_TW`), Enlish (`en_XX`, default).
	 * @param PersonalDataProcessingConsent Whether the user gave consent to processing of their personal data.
	 * @param ReceiveNewsConsent Whether the user gave consent to receive the newsletters.
	 * @param AdditionalFields Parameters used for extended registration forms.
	 * @param SuccessCallback Called after successful user registration. Account confirmation message will be sent to the specified email address.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void RegisterUser(const FString& Username, const FString& Password, const FString& Email, const FString& State, const FString& Locale,
		const bool PersonalDataProcessingConsent, const bool ReceiveNewsConsent, const TMap<FString, FString>& AdditionalFields,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Resends a sign-up confirmation email to the specified email address. To complete registration, the user must follow the link from the email.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/classic-auth/).
	 *
	 * @param Username Username.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param Locale Defines localization of the email the user receives.<br>
	 * The following languages are supported: Arabic (`ar_AE`), Bulgarian (`bg_BG`), Czech (`cz_CZ`), German (`de_DE`), Spanish (`es_ES`), French (`fr_FR`), Hebrew (`he_IL`), Italian (`it_IT`), Japanese (`ja_JP`), Korean (`ko_KR`), Polish (`pl_PL`), Portuguese (`pt_BR`), Romanian (`ro_RO`), Russian (`ru_RU`), Thai (`th_TH`), Turkish (`tr_TR`), Vietnamese (`vi_VN`), Chinese Simplified (`zh_CN`), Chinese Traditional (`zh_TW`), Enlish (`en_XX`, default).
	 * @param SuccessCallback Called after successful sending of the request.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResendAccountConfirmationEmail(const FString& Username, const FString& State, const FString& Locale,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Authenticates the user by the username/email and password specified via the authentication interface.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/classic-auth/).
	 *
	 * @param Username Username or email.
	 * @param Password Password.
	 * @param SuccessCallback Called after successful user authentication. Authentication data including the JWT will be received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is `false`.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticateUser(const FString& Username, const FString& Password,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, const bool bRememberMe = false);

	/** Authenticates the user with Xsolla Login widget.
	 *
	 * @param WorldContextObject The world context.
	 * @param BrowserWidget Widget that shows the social network authentication form. Can be set in the project settings.
	 * @param SuccessCallback Called after successful user authentication. Authentication data including the JWT will be received.
	 * @param CancelCallback Called after user authentication was canceled.
	 * @param bRememberMe Whether the user agrees to save the authentication data. `false` by default.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "SuccessCallback, CancelCallback"))
	void AuthWithXsollaWidget(UObject* WorldContextObject, UXsollaLoginBrowserWrapper*& BrowserWidget,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthCancel& CancelCallback, const bool bRememberMe = false);

	/** Resets the user’s current password and sends an email to change the password to the email address specified during sign-up.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/classic-auth/).
	 *
	 * @param User User identifier (name or email address depending on user data storage type).
	 * @param Locale Defines localization of the email the user receives.<br>
	 * The following languages are supported: Arabic (`ar_AE`), Bulgarian (`bg_BG`), Czech (`cz_CZ`), German (`de_DE`), Spanish (`es_ES`), French (`fr_FR`), Hebrew (`he_IL`), Italian (`it_IT`), Japanese (`ja_JP`), Korean (`ko_KR`), Polish (`pl_PL`), Portuguese (`pt_BR`), Romanian (`ro_RO`), Russian (`ru_RU`), Thai (`th_TH`), Turkish (`tr_TR`), Vietnamese (`vi_VN`), Chinese Simplified (`zh_CN`), Chinese Traditional (`zh_TW`), Enlish (`en_XX`, default).
	 * @param SuccessCallback Called after successful user password reset.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResetUserPassword(const FString& User, const FString& Locale, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Internal request for token validation (called with each auth update automatically)
	 *
	 * @param SuccessCallback Called after successful token validation.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Returns URL for authentication via the specified social network in a browser.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/social-auth/#sdk_how_to_set_up_web_auth_via_social_networks).
	 *
	 * @param ProviderName Name of a social network. Provider must be connected to Login in Publisher Account.<br>
	 * Can be `amazon`, `apple`, `baidu`, `battlenet`, `discord`, `facebook`, `github`, `google`, `kakao`, `linkedin`, `mailru`, `microsoft`, `msn`, `naver`, `ok`, `paypal`, `psn`, `qq`, `reddit`, `steam`, `twitch`, `twitter`, `vimeo`, `vk`, `wechat`, `weibo`, `yahoo`, `yandex`, `youtube`, or `xbox`.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param SuccessCallback Called after URL for social authentication was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetSocialAuthenticationUrl(const FString& ProviderName, const FString& State,
		const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Opens social authentication URL in the browser.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/social-auth/#sdk_how_to_set_up_web_auth_via_social_networks).
	 *
	 * @param WorldContextObject The world context.
	 * @param BrowserWidget Widget to show the social network authentication form. Can be set in the project settings.
	 * @param bRememberMe Whether the user agrees to save the authentication data. `false` by default.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (WorldContext = "WorldContextObject"))
	void LaunchSocialAuthentication(UObject* WorldContextObject, UUserWidget*& BrowserWidget, const bool bRememberMe = false);

	/** Opens the specified social network mobile app (if available) in order to authenticate the user.
	 *
	 * @param ProviderName Name of the social network connected to Login in Publisher Account. Can be `facebook`, `google`, `wechat`, or `qq_mobile`.
	 * @param SuccessCallback Called after successful user authentication. Authentication data including the JWT will be received.
	 * @param CancelCallback Called after user authentication was canceled.
	 * @param ErrorCallback Called after user authentication resulted with an error.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is `false`.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, CancelCallback, ErrorCallback"))
	void LaunchNativeSocialAuthentication(const FString& ProviderName,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthCancel& CancelCallback, const FOnAuthError& ErrorCallback, const bool bRememberMe = false, const FString& State = TEXT("xsollatest"));

	/** Sets a new value of a token (used when the token is obtained via social network authentication, etc.).
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetToken(const FString& Token);

	/** Refreshes the token in case it is expired. Works only when OAuth 2.0 is enabled.
	 *
	 * @param RefreshToken Token used to refresh the expired access token. Received when authorizing the user with username/password for the first time.
	 * @param SuccessCallback Called after successful token refreshing. Refresh data including the JWT will be received.
	 * @param ErrorCallback Called after request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RefreshToken(const FString& RefreshToken, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Exchanges the user authentication code to a valid JWT.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/general-info/#unreal_engine_sdk_how_to_set_up_oauth).
	 *
	 * @param AuthenticationCode Access code received from several other OAuth2.0 requests (example: code from social network authentication).
	 * @param SuccessCallback Called after successful exchanging. Contains exchanged token.
	 * @param ErrorCallback Called after request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ExchangeAuthenticationCodeToToken(const FString& AuthenticationCode, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Authenticates a user by exchanging the session ticket from Steam, Xbox, or Epic Games to the JWT.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/silent-auth/).
	 *
	 * @param ProviderName Platform on which the session ticket was obtained. Can be `steam`, `xbox`, or `epicgames`.
	 * @param SessionTicket Session ticket received from the platform.
	 * @param Code Code received from the platform.
	 * @param AppId Platform application identifier.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param SuccessCallback Called after successful user authentication with a platform session ticket. Authentication data including a JWT will be received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void AuthenticateWithSessionTicket(const FString& ProviderName, const FString& SessionTicket, const FString& Code,
		const FString& AppId, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Returns a list of particular user’s attributes with their values and descriptions. Returns only user-editable attributes.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-attributes/).
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of a user whose public attributes should be requested. If not specified, the method returns attrubutes for the current user.
	 * @param AttributeKeys List of attributes’ keys which you want to get. If not specified, the method returns all user’s attributes.
	 * @param SuccessCallback Called after user attributes were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void GetUserAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnUserAttributesUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns list of user read-only attributes.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of a user whose attributes should be updated.
	 * @param AttributeKeys List of attributes’ keys which you want to get. If not specified, the method returns all user’s attributes.
	 * @param SuccessCallback Called after user attributes were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void GetUserReadOnlyAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnUserAttributesUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Updates the values of user attributes with the specified IDs. The method can be used to create attributes. Changes are made on the user data storage side (server side).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-attributes/).
	 *
	 * @param AuthToken User authorization token.
	 * @param AttributesToModify List of attributes of the specified game.<br>
	 * To add attribute which doesn't exist, set this attribute to the `key` parameter.<br>
	 * To update value of the attribute, specify its `key` parameter and set the new `value`. You can change several attributes at a time.
	 * @param SuccessCallback Called after successful user attributes modification on the server side.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserAttributes(const FString& AuthToken, const TArray<FXsollaUserAttribute>& AttributesToModify,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Removes user attributes with the specified IDs. Changes are made on the user data storage side (server side).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-attributes/).
	 *
	 * @param AuthToken User authorization token.
	 * @param AttributesToRemove List of attribute keys for removal.
	 * @param SuccessCallback Called after successful user attributes removal on the server side.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Checks user age for a particular region. The age requirements depend on the region. Service determines the user location by the IP address.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param DateOfBirth User's birth date in the `YYYY-MM-DD` format.
	 * @param SuccessCallback Called after successful check of the user age.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckUserAge(const FString& DateOfBirth, const FOnCheckUserAgeSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Adds a username, email address, and password, that can be used for authentication, to the current account.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/auth-via-device-id/).
	 *
	 * @param AuthToken User authorization token.
	 * @param Email User email.
	 * @param Password User password.
	 * @param ReceiveNewsConsent Whether the user gave consent to receive the newsletters.
	 * @param Username User's username.
	 * @param SuccessCallback Called after successful email and password linking.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkEmailAndPassword(const FString& AuthToken, const FString& Email, const FString& Password, const bool ReceiveNewsConsent, const FString& Username,
		const FOnLinkEmailAndPasswordSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Links the specified device to the current user account.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/auth-via-device-id/).
	 *
	 * @param AuthToken User authorization token.
	 * @param PlatformName Name of the mobile platform. Can be `android` or `ios`.
	 * @param DeviceName Manufacturer and model name of the device.
	 * @param DeviceId Platform specific unique device ID.<br>
	 * For Android, it is an [ANDROID_ID](https://developer.android.com/reference/android/provider/Settings.Secure#ANDROID_ID) constant.<br>
	 * For iOS, it is an [identifierForVendor](https://developer.apple.com/documentation/uikit/uidevice/1620059-identifierforvendor?language=objc) property.
	 * @param SuccessCallback Called after successful linking of the device.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkDeviceToAccount(const FString& AuthToken, const FString& PlatformName, const FString& DeviceName, const FString& DeviceId,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Unlinks the specified device from the current user account.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/auth-via-device-id/).
	 *
	 * @param AuthToken User authorization token.
	 * @param DeviceId Platform specific unique device ID.<br>
	 * For Android, it is an [ANDROID_ID](https://developer.android.com/reference/android/provider/Settings.Secure#ANDROID_ID) constant.<br>
	 * For iOS, it is an [identifierForVendor](https://developer.apple.com/documentation/uikit/uidevice/1620059-identifierforvendor?language=objc) property.
	 * @param SuccessCallback Called after successful unlinking of the device.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UnlinkDeviceFromAccount(const FString& AuthToken, const int64 DeviceId,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Authenticates the user via a particular device ID.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/auth-via-device-id/).
	 *
	 * @param DeviceName Manufacturer and model name of the device.
	 * @param DeviceId Platform specific unique device ID.<br>
	 * For Android, it is an [ANDROID_ID](https://developer.android.com/reference/android/provider/Settings.Secure#ANDROID_ID) constant.<br>
	 * For iOS, it is an [identifierForVendor](https://developer.apple.com/documentation/uikit/uidevice/1620059-identifierforvendor?language=objc) property.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param SuccessCallback Called after successful user authentication via the device ID.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticateViaDeviceId(const FString& DeviceName, const FString& DeviceId, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Authenticates the user with the access token using social network credentials.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/social-auth/#unreal_engine_sdk_how_to_set_up_native_auth_via_social_networks).
	 *
	 * @param AuthToken Access token received from a social network.
	 * @param AuthTokenSecret Parameter `oauth_token_secret` received from the authorization request. Required for Twitter only.
	 * @param OpenId Parameter `openid` received from the social network. Required for WeChat only.
	 * @param ProviderName Name of the social network connected to Login in Publisher Account. Can be `facebook`, `google`, `wechat`, or `qq_mobile`.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param SuccessCallback Called after successful user authentication on the specified platform.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthViaAccessTokenOfSocialNetwork(const FString& AuthToken, const FString& AuthTokenSecret, const FString& OpenId,
		const FString& ProviderName, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Starts user authentication and sends an SMS with a one-time code and a link to the specified phone number (if login via magic link is configured for the Login project).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/passwordless-auth/).
	 *
	 * @param PhoneNumber User phone number.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param SuccessCallback Called after successful phone number authentication start.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void StartAuthByPhoneNumber(const FString& PhoneNumber, const FString& State,
		const FOnStartAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Completes authentication after the user enters a one-time code or follows a link received by SMS.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/passwordless-auth/).
	 *
	 * @param Code Confirmation code.
	 * @param OperationId Identifier of the confirmation code.
	 * @param PhoneNumber User phone number.
	 * @param SuccessCallback Called after successful phone number authentication.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CompleteAuthByPhoneNumber(const FString& Code, const FString& OperationId, const FString& PhoneNumber,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Starts user authentication and sends an email with a one-time code and a link to the specified email address (if login via magic link is configured for the Login project).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/passwordless-auth/).
	 *
	 * @param Email User email address.
	 * @param State Value used for additional user verification on backend. Must be at least 8 symbols long. `xsollatest` by default. Required for OAuth 2.0.
	 * @param SuccessCallback Called after successful email authentication start.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void StartAuthByEmail(const FString& Email, const FString& State,
		const FOnStartAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Completes authentication after the user enters a one-time code or follows a link received in an email.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/passwordless-auth/).
	 *
	 * @param Code Confirmation code.
	 * @param OperationId Identifier of the confirmation code.
	 * @param Email User email address.
	 * @param SuccessCallback Called after successful email authentication.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CompleteAuthByEmail(const FString& Code, const FString& OperationId, const FString& Email,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Returns confirmation code for completing authentication via email or phone number. User must follow the link provided via email/SMS to receive the code.
	 *
	 * @param UserId Identifier of the user (can be either email or phone number).
	 * @param OperationId Identifier of the confirmation code.
	 * @param SuccessCallback Called after receiving the confirmation code.
	 * @param TimeoutCallback Called after request timeout.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, TimeoutCallback, ErrorCallback"))
	void GetAuthConfirmationCode(const FString& UserId, const FString& OperationId,
		const FOnAuthCodeSuccess& SuccessCallback, const FOnAuthCodeTimeout& TimeoutCallback, const FOnAuthError& ErrorCallback);

	/** Returns user details.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Called after successful user details were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserDetails(const FString& AuthToken, const FOnUserDetailsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Updates the specified user’s information. Changes are made on the user data storage side.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param Birthday User birth date in format `YYYY-MM-DD`. Can be changed only once.
	 * @param FirstName User first name. Pass empty string to remove the current first name.
	 * @param LastName User last name. Pass empty string to remove the current last name.
	 * @param Gender User gender (`f` - for female, `m` - for male).
	 * @param Nickname User nickname. Pass empty string to remove the current nickname.
	 * @param SuccessCallback Called after successful user details modification.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserDetails(const FString& AuthToken, const FString& Birthday, const FString& FirstName, const FString& LastName, const FString& Gender, const FString& Nickname,
		const FOnUserDetailsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns the user’s email.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Called after user email was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserEmail(const FString& AuthToken, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns user phone number that is used for two-factor authentication.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Called after user phone number was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserPhoneNumber(const FString& AuthToken, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Changes the user’s phone number that is used for two-factor authentication. Changes are made on the user data storage side (server-side).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber New user phone number.
	 * @param SuccessCallback Called after user phone number was successfully modified.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Deletes the user’s phone number that is used for two-factor authentication. Changes are made on the user data storage side (server side).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber User phone number for removal.
	 * @param SuccessCallback Called after the user phone number was successfully removed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Changes the user’s avatar. Changes are made on the user data storage side (server side).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param Picture New user profile picture.
	 * @param SuccessCallback Called after the user profile picture was successfully modified.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserProfilePicture(const FString& AuthToken, const UTexture2D* const Picture, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Deletes the user’s avatar. Changes are made on the user data storage side (server side).
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/user-account/).
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Called after user profile picture was successfully removed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveProfilePicture(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns user friends data.
	 *
	 * @param AuthToken User authorization token.
	 * @param Type Friends type.
	 * @param SortBy Condition for sorting users (by name/by update).
	 * @param SortOrder Condition for sorting users (ascending/descending).
	 * @param SuccessCallback Called after user friends data was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param After Parameter that is used for API pagination.
	 * @param Limit Maximum number of friends that can be received at a time.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetFriends(const FString& AuthToken, const EXsollaFriendsType Type, const EXsollaUsersSortCriteria SortBy, const EXsollaUsersSortOrder SortOrder,
		const FOnUserFriendsUpdate& SuccessCallback, const FOnError& ErrorCallback, const FString& After, const int Limit = 20);

	/** Modifies relationships with the specified user.
	 *
	 * @param AuthToken User authorization token.
	 * @param Action Type of action to be applied to a specified friend.
	 * @param UserID Identifier of a user to change relationships with.
	 * @param SuccessCallback Called after successful user friends data modification.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyFriends(const FString& AuthToken, const EXsollaFriendAction Action, const FString& UserID, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns list of links for social authentication enabled in Publisher Account.
	 *
	 * @param AuthToken User authorization token.
	 * @param Locale Region in the `<language code>_<country code>` format, where:
	 * - `language code` — language code in the [ISO 639-1](https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes) format;
	 * - `country code` — country/region code in the [ISO 3166-1 alpha-2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2) format.<br>
	 * 	The list of the links will be sorted from most to least used social networks, according to the variable value.
	 * @param SuccessCallback Called after list of links for social authentication was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSocialAuthLinks(const FString& AuthToken, const FString& Locale, const FOnSocialAuthLinksUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns user friends data from a social provider.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of social provider. If empty, friends from all available social providers will be fetched.
	 * @param SuccessCallback Called after user friends data was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated.
	 * @param Limit Maximum number of friends that can be received at a time.
	 * @param FromThisGame Flag indicating whether social friends are from this game.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSocialFriends(const FString& AuthToken, const FString& Platform,
		const FOnUserSocialFriendsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Offset = 0, const int Limit = 500, const bool FromThisGame = false);

	/** Returns friends on the server.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of the chosen social provider. If not specified, the method gets friends from all social providers.
	 * @param SuccessCallback Called after user friends were successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUsersFriends(const FString& AuthToken, const FString& Platform, const FOnCodeReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns specified user public profile information.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserID User identifier of public profile information to be received.
	 * @param SuccessCallback Called after user profile data was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserProfile(const FString& AuthToken, const FString& UserID, const FOnUserProfileReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns a list of devices linked to the current user account.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/authentication/auth-via-device-id/).
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Called after users devices data was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUsersDevices(const FString& AuthToken, const FOnUserDevicesUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Searches for users with the specified nickname.
	 *
	 * @param AuthToken User authorization token.
	 * @param Nickname User nickname used as search criteria.
	 * @param SuccessCallback Called after user search is successfully completed.
	 * @param ErrorCallback Called after the request resulted with an error.
	 * @param Offset Number of elements from which the list is generated.
	 * @param Limit Maximum number of users that can be received at a time.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void SearchUsersByNickname(const FString& AuthToken, const FString& Nickname,
		const FOnUserSearchUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Offset = 0, const int Limit = 100);

	/** Links a social network that can be used for authentication to the current account.
	 * [More about the use cases](https://developers.xsolla.com/sdk/unreal-engine/user-account-and-attributes/account-linking/#sdk_account_linking_additional_account).
	 *
	 * @param AuthToken User authorization token.
	 * @param ProviderName Name of a social network. Provider must be connected to Login in Publisher Account.<br>
	 * Can be `amazon`, `apple`, `baidu`, `battlenet`, `discord`, `facebook`, `github`, `google`, `instagram`, `kakao`, `linkedin`, `mailru`, `microsoft`, `msn`, `naver`, `ok`, `paradox`, `paypal`, `psn`, `qq`, `reddit`, `steam`, `twitch`, `twitter`, `vimeo`, `vk`, `wechat`, `weibo`, `yahoo`, `yandex`, `youtube`, `xbox`, `playstation`.
	 * @param SuccessCallback Called after the URL for social authentication was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkSocialNetworkToUserAccount(const FString& AuthToken, const FString& ProviderName,
		const FOnSocialAccountLinkingHtmlReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Returns the list of linked social networks.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Called after the list of linked social networks was successfully received.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetLinkedSocialNetworks(const FString& AuthToken, const FOnLinkedSocialNetworksUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Logs the user out and deletes the user session according to the value of the sessions parameter (OAuth2.0 only).
	 *
	 * @param AuthToken User authorization token.
	 * @param Sessions Shows how the user is logged out and how the user session is deleted. Available strings: `sso` and `all`. Leave empty to use the default value (all).
	 * @param SuccessCallback Called after successful user logout.
	 * @param ErrorCallback Called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LogoutUser(const FString& AuthToken, const EXsollaSessionType Sessions,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

protected:
	void Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void DefaultWithHandlerWrapper_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SocialAuthUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnSocialUrlReceived SuccessCallback, FOnAuthError ErrorCallback);
	void GetUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserAttributesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetReadOnlyUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserAttributesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void CheckUserAge_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnCheckUserAgeSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void DeviceId_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void RefreshToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void InnerRefreshToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnLoginDataUpdate SuccessCallback, FOnLoginDataError ErrorCallback);
	void SessionTicket_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void AuthViaAccessTokenOfSocialNetwork_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void StartAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnStartAuthSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void CompleteAuthByPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void CompleteAuthByEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void GetAuthConfirmationCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthCodeSuccess SuccessCallback, FOnAuthCodeTimeout TimeoutCallback, FOnAuthError ErrorCallback);
	void UserDetails_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserDetailsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void ModifyPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void RemovePhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserProfilePicture_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserDetailsParamUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserProfilePictureRemove_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserFriendsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void SocialAuthLinks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnSocialAuthLinksUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void SocialFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserSocialFriendsUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetUsersFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnCodeReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserProfile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserProfileReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void UserSearch_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserSearchUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void SocialAccountLinking_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnSocialAccountLinkingHtmlReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void LinkedSocialNetworks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnLinkedSocialNetworksUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetUsersDevices_HttpRequestComplete(const FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnUserDevicesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void LinkEmailAndPassword_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnLinkEmailAndPasswordSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void RegisterUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void LogoutUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);

	/** Processes the request for obtaining/refreshing token using OAuth 2.0. */
	void HandleOAuthTokenRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError& ErrorCallback, FOnAuthUpdate& SuccessCallback);

	/** Processes the request that returns URL with a code that can be exchanged to user token (OAuth 2.0). */
	void HandleUrlWithCodeRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);

	/** Returns true if the error occurs. */
	void HandleRequestOAuthError(XsollaHttpRequestError ErrorData, FOnAuthError ErrorCallback);

	void InnerRefreshToken(const FString& RefreshToken, const FOnLoginDataUpdate& SuccessCallback, const FOnLoginDataError& ErrorCallback);

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET,
		const FString& Content = FString(), const FString& AuthToken = FString());

	/** Sets a JSON string array field named FieldName and value of Array. */
	void SetStringArrayField(TSharedPtr<FJsonObject> Object, const FString& FieldName, const TArray<FString>& Array) const;

	/** Cached Xsolla project ID. */
	FString ProjectID;

	/** Cached Xsolla Login project. */
	FString LoginID;

	/** Cached Xsolla client ID. */
	FString ClientID;

public:
	/** Returns user login state data. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Login")
	FXsollaLoginData GetLoginData() const;

	/** Sets user login state data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetLoginData(const FXsollaLoginData& Data, const bool ClearCache = true);

	/** Updates user auth token data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void UpdateAuthTokenData(const FString& AccessToken, int ExpiresIn, const FString& RefreshToken, bool bRememberMe = true);

	/** Drops cache and cleans login data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void DropLoginData(const bool ClearCache = true);

	/** Loads save game and extract data. */
	void LoadSavedData();

	/** Saves cached data or resets it if RememberMe is false. */
	void SaveData();

	void HandleRequestError(const XsollaHttpRequestError& ErrorData, FErrorHandlersWrapper ErrorHandlersWrapper);
protected:
	/** Keeps state of user login. */
	FXsollaLoginData LoginData;

private:
	UPROPERTY()
	TSubclassOf<UXsollaLoginBrowserWrapper> DefaultBrowserWidgetClass;

	UPROPERTY()
	FOnAuthUpdate NativeSuccessCallback;

	UPROPERTY()
	FOnAuthCancel NativeCancelCallback;

	UPROPERTY()
	FOnAuthError NativeErrorCallback;
};
