// Copyright 2021 Xsolla Inc. All Rights Reserved.

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
	 * @param InProjectId New Project ID value from Publisher Account > Project settings > Project ID.
	 * @param InLoginId New Login ID value from Publisher Account > Login settings.
	 * @param InClientId New Client ID value from Publisher Account > Login settings -> OAuth 2.0 authentication settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void Initialize(const FString& InProjectId, const FString& InLoginId, const FString& InClientId);

	// textreview
	/** Sign up User
	 * Adds a new user to the database. The user will receive an account confirmation message to the specified email.
	 *
	 * @param Username Username. Required.
	 * @param Password Password. Required.
	 * @param Email Email. Required.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param Locale Defines localization of the email user receives.
	 * @param PersonalDataProcessingConsent Whether the user gave consent to processing of their personal data.
	 * @param ReceiveNewsConsent Whether the user gave consent to receive the newsletters.
	 * @param AdditionalFields Parameters used for extended registration forms.
	 * @param SuccessCallback Callback function called after successful user registration. Account confirmation message will be send to the specified email.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void RegisterUser(const FString& Username, const FString& Password, const FString& Email, const FString& State, const FString& Locale,
		const bool PersonalDataProcessingConsent, const bool ReceiveNewsConsent, const TMap<FString, FString>& AdditionalFields,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	// textreview
	/** Resend Account Confirmation Email
	 * Resends an account confirmation email to a user. To complete account confirmation, the user should follow the link in the email.
	 *
	 * @param Username Username. Required.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param Locale Defines localization of the email user receives.
	 * @param SuccessCallback Callback function called after successful request sending.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResendAccountConfirmationEmail(const FString& Username, const FString& State, const FString& Locale,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Authenticate User
	 * Authenticates the user by the username and password specified via the authentication interface.
	 *
	 * @param Username Username. Required.
	 * @param Password Password. Required.
	 * @param SuccessCallback Callback function called after successful user authentication. Authentication data including the JWT will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticateUser(const FString& Username, const FString& Password,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, const bool bRememberMe = false);

	// textreview
	/** Reset User Password
	 * Resets the user password.
	 *
	 * @param User User identifier (name or email depending on user data storage type). Required.
	 * @param Locale Defines localization of the email user receives.
	 * @param SuccessCallback Callback function called after successful user password reset.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResetUserPassword(const FString& User, const FString& Locale, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Internal request for token validation (called with each auth update automatically)
	 *
	 * @param SuccessCallback Callback function called after successful token validation.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ValidateToken(const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Get Social Authentication URL
	 * Gets URL for authentication via the specified social network.
	 *
	 * @param ProviderName Name of a social network. Provider must be connected to Login in Publisher Account. Required.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param SuccessCallback Callback function called after URL for social authentication was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetSocialAuthenticationUrl(const FString& ProviderName, const FString& State,
		const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Launch social authentication
	 * Opens social authentication URL in the browser.
	 *
	 * @param WorldContextObject The world context.
	 * @param BrowserWidget Widget to show the social network authentication form. Can be set in the project settings.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (WorldContext = "WorldContextObject"))
	void LaunchSocialAuthentication(UObject* WorldContextObject, UUserWidget*& BrowserWidget, const bool bRememberMe = false);

	/** Launch native authentication via social network
	 * Opens the specified social network mobile app (if available) in order to authenticate the user.
	 *
	 * @param ProviderName Name of a social network. Provider must be connected to Login in Publisher Account. Required.
	 * @param SuccessCallback Callback function called after successful user authentication. Authentication data including the JWT will be received.
	 * @param CancelCallback Callback function called after user authentication was canceled.
	 * @param ErrorCallback Callback function called after user authentication resulted with an error.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, CancelCallback, ErrorCallback"))
	void LaunchNativeSocialAuthentication(const FString& ProviderName,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthCancel& CancelCallback, const FOnAuthError& ErrorCallback, const bool bRememberMe = false);

	/** Sets a new value of a token (used when the token is obtained via social network authentication, etc.).
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetToken(const FString& Token);

	/** Refresh authentication token
	 * Refreshes the token in case it is expired. Works only when OAuth 2.0 is enabled.
	 *
	 * @param RefreshToken Token used to refresh the expired access token. Received when authorizing the user with username/password for the first time.
	 * @param SuccessCallback Callback function called after successful token refreshing. Refresh data including the JWT will be received.
	 * @param ErrorCallback Callback function called after request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RefreshToken(const FString& RefreshToken, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Exchange code to token
	 * Exchanges the user authentication code to a valid JWT.
	 *
	 * @param AuthenticationCode User authentication code to be exchanged to a JWT.
	 * @param SuccessCallback Callback function called after successful exchanging.
	 * @param ErrorCallback Callback function called after request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ExchangeAuthenticationCodeToToken(const FString& AuthenticationCode, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Authenticate With Session Ticket
	 * Authenticates a user by exchanging the platform specific session ticket for a token.
	 *
	 * @param ProviderName Platform on which the session ticket was obtained.
	 * @param SessionTicket Session ticket.
	 * @param Code Code received from the platform.
	 * @param AppId Platform application identifier.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param SuccessCallback Callback function called after successful user authentication with a platform session ticket. Authentication data including a JWT will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void AuthenticateWithSessionTicket(const FString& ProviderName, const FString& SessionTicket, const FString& Code,
		const FString& AppId, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Get User Attributes
	 * Gets the list of user attributes.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of a user whose attributes should be requested.
	 * @param AttributeKeys Keys of the attributes that should be requested.
	 * @param SuccessCallback Callback function called after user attributes were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void GetUserAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnUserAttributesUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get User Read-Only Attributes
	 * Gets list of user read-only attributes.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of a user whose attributes should be updated.
	 * @param AttributeKeys Keys of the attributes that should be updated.
	 * @param SuccessCallback Callback function called after user attributes were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void GetUserReadOnlyAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnUserAttributesUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Modify User Attributes
	 * Modifies the list of user attributes by creating/editing its items (changes made on the server side).
	 *
	 * @param AuthToken User authorization token.
	 * @param AttributesToModify List of new/edited attributes.
	 * @param SuccessCallback Callback function called after successful user attributes modification on the server side.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserAttributes(const FString& AuthToken, const TArray<FXsollaUserAttribute>& AttributesToModify,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Remove User Attributes
	 * Removes user attributes with specified keys (changes made on the server side).
	 *
	 * @param AuthToken User authorization token.
	 * @param AttributesToRemove List of attribute keys for removal.
	 * @param SuccessCallback Callback function called after successful user attributes removal on the server side.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Create Code for Linking Accounts
	 * Creates code for linking the user platform account to the main account.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful account linking code creation. The new linking code will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateAccountLinkingCode(const FString& AuthToken, const FOnCodeReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Check User Age
	 * Checks user age for a particular region. The age requirements depend on the region. Service determines the user location by the IP address.
	 *
	 * @param DateOfBirth User's birth date in the 'YYYY-MM-DD' format.
	 * @param SuccessCallback Callback function called after successful check of the user age.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckUserAge(const FString& DateOfBirth, const FOnCheckUserAgeSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Link Accounts by Code
	 * Links the user platform account to the existing main account by the code.
	 *
	 * @param UserId User identifier from a platform account.
	 * @param Platform Platform type.
	 * @param Code Account linking code obtained from the master account.
	 * @param SuccessCallback Callback function called after successful account linking.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkAccount(const FString& UserId, const EXsollaPublishingPlatform Platform, const FString& Code,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Link Email, Password, and Username to Account
	 * Adds the username/email and password authentication to the existing user account. This call is used if the account is created via the device ID or phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param Email User email.
	 * @param Password User password.
	 * @param ReceiveNewsConsent Whether the user gave consent to receive the newsletters.
	 * @param Username User's username.
	 * @param SuccessCallback Callback function called after successful email and password linking.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkEmailAndPassword(const FString& AuthToken, const FString& Email, const FString& Password, const bool ReceiveNewsConsent, const FString& Username,
		const FOnLinkEmailAndPasswordSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Link Device to Account
	 * Links the specified device to the user account.
	 *
	 * @param AuthToken User authorization token.
	 * @param PlatformName Name of the mobile platform. Can be Android or iOS.
	 * @param DeviceName Name of mobile device.
	 * @param DeviceId Platform specific unique device ID.
	 * @param SuccessCallback Callback function called after successful linking of the device.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkDeviceToAccount(const FString& AuthToken, const FString& PlatformName, const FString& DeviceName, const FString& DeviceId,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Unlink Device from Account
	 * Unlinks the specified device from the user account.
	 *
	 * @param AuthToken User authorization token.
	 * @param DeviceId Platform specific unique device ID. It is generated by the Xsolla Login server. It is not the same as the device_id parameter from the Auth via device ID call.
	 * @param SuccessCallback Callback function called after successful unlinking of the device.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UnlinkDeviceFromAccount(const FString& AuthToken, const int64 DeviceId,
		const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Cross-Authenticate
	 * Authenticates a platform account user.
	 *
	 * @param UserId User identifier from a platform account.
	 * @param Platform Platform type.
	 * @param InvalidateExistingSessions 
	 * @param SuccessCallback Callback function called after successful user authentication on a specified platform.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticatePlatformAccountUser(const FString& UserId, const EXsollaPublishingPlatform Platform, bool InvalidateExistingSessions, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Cross-Authenticate
	 * Authenticates a platform account user via deviceId.
	 *
	 * @param DeviceName Name of the mobile device.
	 * @param DeviceId Platform specific unique device ID.
	 * @param State Value used for additional user verification. Often used to mitigate CSRF attacks. The value will be returned in the response. Must be longer than 8 characters.
	 * @param SuccessCallback Callback function called after successful user authentication via the device ID.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticateViaDeviceId(const FString& DeviceName, const FString& DeviceId, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Auth Via Access Token of Social Network
	 * Authenticates the user with the access token using social network credentials.
	 *
	 * @param AuthToken Access token received from a social network
	 * @param AuthTokenSecret Parameter 'oauth_token_secret' received from the authorization request. Required for Twitter only.
	 * @param OpenId Parameter 'openid' received from the social network. Required for WeChat only.
	 * @param ProviderName Name of the social network connected to Login in Publisher Account. Can have the following values: 'facebook', 'google', 'linkedin', 'twitter', 'discord', 'naver', and 'baidu'.
	 * @param State Value used for additional user verification. Often used to mitigate CSRF Attacks. The value will be returned in the response. Must be longer than 8 symbols.
	 * @param SuccessCallback Callback function called after successful user authentication on the specified platform.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthViaAccessTokenOfSocialNetwork(const FString& AuthToken, const FString& AuthTokenSecret, const FString& OpenId,
		const FString& ProviderName, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Start Auth by Phone Number
	 * Starts authentication by the user phone number and sends a confirmation code to their phone number.
	 *
	 * @param PhoneNumber User phone number.
	 * @param State Value used for additional user verification. Often used to mitigate CSRF attacks. The value will be returned in the response. Must be longer than 8 characters.
	 * @param SuccessCallback Callback function called after successful phone number authentication start.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void StartAuthByPhoneNumber(const FString& PhoneNumber, const FString& State,
		const FOnStartAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Complete Auth by Phone Number
	 * Completes authentication by the user phone number and a confirmation code.
	 *
	 * @param Code Confirmation code.
	 * @param OperationId ID of the confirmation code.
	 * @param PhoneNumber User phone number.
	 * @param SuccessCallback Callback function called after successful phone number authentication.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CompleteAuthByPhoneNumber(const FString& Code, const FString& OperationId, const FString& PhoneNumber,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Start Auth by Email
	 * Starts authentication by the user email address and sends a confirmation code to their email address.
	 *
	 * @param Email User email address.
	 * @param State Value used for additional user verification. Often used to mitigate CSRF attacks. The value will be returned in the response. Must be longer than 8 characters.
	 * @param SuccessCallback Callback function called after successful email authentication start.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void StartAuthByEmail(const FString& Email, const FString& State,
		const FOnStartAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Complete Auth by Email
	 * Completes authentication by the user email address and a confirmation code.
	 *
	 * @param Code Confirmation code.
	 * @param OperationId ID of the confirmation code.
	 * @param Email User email address.
	 * @param SuccessCallback Callback function called after successful email authentication.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CompleteAuthByEmail(const FString& Code, const FString& OperationId, const FString& Email,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Get Auth Confirmation Code
	 * Gets confirmation code for completing authentication via email or phone number. User must follow the link provided via email/SMS to receive the code.
	 *
	 * @param UserId Identifier of the user (can be either email or phone number).
	 * @param OperationId ID of the confirmation code.
	 * @param SuccessCallback Callback function called after receiving the confirmation code.
	 * @param TimeoutCallback Callback function called after request timeout.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, TimeoutCallback, ErrorCallback"))
	void GetAuthConfirmationCode(const FString& UserId, const FString& OperationId,
		const FOnAuthCodeSuccess& SuccessCallback, const FOnAuthCodeTimeout& TimeoutCallback, const FOnAuthError& ErrorCallback);

	/** Get User Details
	 * Gets user details.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful user details were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserDetails(const FString& AuthToken, const FOnUserDetailsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Modify User Details
	 * Modifies specified user details.
	 *
	 * @param AuthToken User authorization token.
	 * @param Birthday User birth date in format (YYYY-MM-DD). Can be changed only once.
	 * @param FirstName User first name. Pass empty string to remove the current first name.
	 * @param LastName User last name. Pass empty string to remove the current last name.
	 * @param Gender User gender (f - for female, m - for male).
	 * @param Nickname User nickname. Pass empty string to remove the current nickname.
	 * @param SuccessCallback Callback function called after successful user details modification.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserDetails(const FString& AuthToken, const FString& Birthday, const FString& FirstName, const FString& LastName, const FString& Gender, const FString& Nickname,
		const FOnUserDetailsUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get User Email
	 * Gets user email.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after user email was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserEmail(const FString& AuthToken, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get User Phone Number
	 * Gets user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after user phone number was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserPhoneNumber(const FString& AuthToken, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Modify User Phone Number
	 * Modifies user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber New user phone number.
	 * @param SuccessCallback Callback function called after user phone number was successfully modified.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Remove User Phone Number
	 * Removes the user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber User phone number for removal.
	 * @param SuccessCallback Callback function called after the user phone number was successfully removed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Modify User Profile Picture
	 * Modifies user profile picture.
	 *
	 * @param AuthToken User authorization token.
	 * @param Picture New user profile picture.
	 * @param SuccessCallback Callback function called after the user profile picture was successfully modified.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserProfilePicture(const FString& AuthToken, const UTexture2D* const Picture, const FOnUserDetailsParamUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Remove User Profile Picture
	 * Removes user profile picture.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after user profile picture was successfully removed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveProfilePicture(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Friends
	 * Gets user friends data.
	 *
	 * @param AuthToken User authorization token.
	 * @param Type Friends type.
	 * @param SortBy Condition for sorting users (by name/by update).
	 * @param SortOrder Condition for sorting users (ascending/descending).
	 * @param SuccessCallback Callback function called after user friends data was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param After Parameter that is used for API pagination.
	 * @param Limit Maximum number of friends that can be received at a time.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetFriends(const FString& AuthToken, const EXsollaFriendsType Type, const EXsollaUsersSortCriteria SortBy, const EXsollaUsersSortOrder SortOrder,
		const FOnUserFriendsUpdate& SuccessCallback, const FOnError& ErrorCallback, const FString& After, const int Limit = 20);

	/** Modify Friends
	 * Modifies relationships with the specified user.
	 *
	 * @param AuthToken User authorization token.
	 * @param Action Type of action to be applied to a specified friend.
	 * @param UserID Identifier of a user to change relationships with.
	 * @param SuccessCallback Callback function called after successful user friends data modification.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyFriends(const FString& AuthToken, const EXsollaFriendAction Action, const FString& UserID, const FOnRequestSuccess& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Social Authentication Links
	 * Gets list of links for social authentication enabled in Publisher Account.
	 *
	 * @param AuthToken User authorization token.
	 * @param Locale Locale.
	 * @param SuccessCallback Callback function called after list of links for social authentication was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSocialAuthLinks(const FString& AuthToken, const FString& Locale, const FOnSocialAuthLinksUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Social Friends
	 * Gets user friends data from a social provider.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of social provider. If empty, friends from all available social providers will be fetched.
	 * @param SuccessCallback Callback function called after user friends data was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated.
	 * @param Limit Maximum number of friends that can be received at a time.
	 * @param FromThisGame Flag indicating whether social friends are from this game.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSocialFriends(const FString& AuthToken, const FString& Platform,
		const FOnUserSocialFriendsUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Offset = 0, const int Limit = 500, const bool FromThisGame = false);

	/** Get Users Friends
	 * Gets friends on the server.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of the chosen social provider. If not specified, the method gets friends from all social providers.
	 * @param SuccessCallback Callback function called after user friends were successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUsersFriends(const FString& AuthToken, const FString& Platform, const FOnCodeReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Get User Profile
	 * Gets specified user public profile information.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserID User identifier of public profile information to be received.
	 * @param SuccessCallback Callback function called after user profile data was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserProfile(const FString& AuthToken, const FString& UserID, const FOnUserProfileReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Users Devices
	 * Gets a list of user’s devices.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after users devices data was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUsersDevices(const FString& AuthToken, const FOnUserDevicesUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Search Users by Nickname
	 * Searches for users with the specified nickname.
	 *
	 * @param AuthToken User authorization token.
	 * @param Nickname User nickname used as search criteria.
	 * @param SuccessCallback Callback function called after user search is successfully completed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of elements from which the list is generated.
	 * @param Limit Maximum number of users that can be received at a time.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void SearchUsersByNickname(const FString& AuthToken, const FString& Nickname,
		const FOnUserSearchUpdate& SuccessCallback, const FOnError& ErrorCallback, const int Offset = 0, const int Limit = 100);

	/** Link Social Network To User's Account
	 * Links the social network, which is used by the player for authentication, to the user account.
	 *
	 * @param AuthToken User authorization token.
	 * @param ProviderName Name of a social network. Provider must be connected to Login in Publisher Account. Required.
	 * @param SuccessCallback Callback function called after the URL for social authentication was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkSocialNetworkToUserAccount(const FString& AuthToken, const FString& ProviderName,
		const FOnSocialAccountLinkingHtmlReceived& SuccessCallback, const FOnError& ErrorCallback);

	/** Get Linked Social Networks
	 * Gets the list of linked social networks.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after the list of linked social networks was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetLinkedSocialNetworks(const FString& AuthToken, const FOnLinkedSocialNetworksUpdate& SuccessCallback, const FOnError& ErrorCallback);

	/** Log Out User
	 * Logs the user out and deletes the user session according to the value of the sessions parameter (OAuth2.0 only).
	 *
	 * @param AuthToken User authorization token.
	 * @param Sessions Shows how the user is logged out and how the user session is deleted. Available strings: 'sso' and 'all'. Leave empty to use the default value (all).
	 * @param SuccessCallback Callback function called after successful user logout.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
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
	void CrossAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void GetUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserAttributesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void GetReadOnlyUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnUserAttributesUpdate SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void AccountLinkingCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnCodeReceived SuccessCallback, FErrorHandlersWrapper ErrorHandlersWrapper);
	void CheckUserAge_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnCheckUserAgeSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void AuthConsoleAccountUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
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

	/** Processes the request that returns URL with user token (JWT). */
	void HandleUrlWithTokenRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
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
	/** Gets user login state data. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Login")
	FXsollaLoginData GetLoginData() const;

	/** Sets user login state data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetLoginData(const FXsollaLoginData& Data, const bool ClearCache = true);

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
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;
};
