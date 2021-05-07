// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginTypes.h"

#include "XsollaUtilsDataModel.h"
#include "XsollaUtilsHttpRequestHelper.h"

#include "Blueprint/UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

#include "XsollaLoginSubsystem.generated.h"

class FJsonObject;

/** Common callback for operations without any user-friendly messages from the server in case of success. */
DECLARE_DYNAMIC_DELEGATE(FOnRequestSuccess);

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
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void Initialize(const FString& InProjectId, const FString& InLoginId);

	/** Sign up User
	 * Adds a new user to the database. The user will receive an account confirmation message to the specified email.
	 *
	 * @param Username Username. Required.
	 * @param Password Password. Required.
	 * @param Email Email. Required.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param Payload Your custom data. The value of the parameter will be returned in the user JWT > payload claim (JWT only).
	 * @param PersonalDataProcessingConsent Whether the user gave consent to processing of their personal data.
	 * @param ReceiveNewsConsent Whether the user gave consent to receive the newsletters.
	 * @param AdditionalFields Parameters used for extended registration forms.
	 * @param SuccessCallback Callback function called after successful user registration. Account confirmation message will be send to the specified email.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void RegisterUser(const FString& Username, const FString& Password, const FString& Email, const FString& State, const FString& Payload,
		bool PersonalDataProcessingConsent, bool ReceiveNewsConsent, TArray<FString> AdditionalFields,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Resend Account Confirmation Email
	 * Resends an account confirmation email to a user. To complete account confirmation, the user should follow the link in the email.
	 *
	 * @param Username Username. Required.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param Payload Your custom data. The value of the parameter will be returned in the user JWT > payload claim (JWT only).
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResendAccountConfirmationEmail(const FString& Username, const FString& State, const FString& Payload,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Authenticate User
	 * Authenticates the user by the username and password specified via the authentication interface.
	 *
	 * @param Username Username. Required.
	 * @param Password Password. Required.
	 * @param Payload Your custom data. The value of the parameter will be returned in the user JWT > payload claim (JWT only).
	 * @param SuccessCallback Callback function called after successful user authentication. Authentication data including the JWT will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticateUser(const FString& Username, const FString& Password, const FString& Payload,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, bool bRememberMe = false);

	/** Reset User Password
	 * Resets the user password.
	 *
	 * @param User User identifier (name or email depending on user data storage type). Required.
	 * @param SuccessCallback Callback function called after successful user password reset.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ResetUserPassword(const FString& User, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

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
	 * @param Payload Your custom data. The value of the parameter will be returned in the user JWT > payload claim (JWT only).
	 * @param AdditionalFields List of parameters which should be requested from the user or social network additionally and written to the token (JWT only).
	 * @param SuccessCallback Callback function called after URL for social authentication was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AdditionalFields, SuccessCallback, ErrorCallback"))
	void GetSocialAuthenticationUrl(const FString& ProviderName, const FString& State, const FString& Payload, const TArray<FString>& AdditionalFields,
		const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Launch social authentication
	 * Opens social authentication URL in the browser.
	 *
	 * @param SocialAuthenticationUrl URL with the social network authentication form.
	 * @param BrowserWidget Widget to show the social network authentication form. Can be set in the project settings.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void LaunchSocialAuthentication(const FString& SocialAuthenticationUrl, UUserWidget*& BrowserWidget, bool bRememberMe = false);

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
		const FOnAuthUpdate& SuccessCallback,const FOnAuthCancel& CancelCallback, const FOnAuthError& ErrorCallback, bool bRememberMe = false);

	/** Sets a new value of a token (used when the token is obtained via social network authentication, etc.).
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetToken(const FString& Token);

	/** Refreshes the token in case it is expired. Works only when OAuth 2.0 is enabled.
	 *
	 * @param RefreshToken Token used to refresh the expired access token. Received when authorizing the user with username/password for the first time.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RefreshToken(const FString& RefreshToken, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Exchange the user authentication code to a valid JWT. Works only when OAuth 2.0 is enabled.
	 *
	 * @param AuthenticationCode User authentication code to be exchanged to a JWT.
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
	 * @param Payload Your custom data. The value of the parameter will be returned in the user JWT > payload claim (JWT only).
	 * @param SuccessCallback Callback function called after successful user authentication with a platform session ticket. Authentication data including a JWT will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void AuthenticateWithSessionTicket(const FString& ProviderName, const FString& SessionTicket, const FString& Code,
		const FString& AppId, const FString& State, const FString& Payload,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update User Attributes
	 * Updates the locally cached list of user attributes.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of a user whose attributes should be updated.
	 * @param AttributeKeys Keys of the attributes that should be updated.
	 * @param SuccessCallback Callback function called after successful user attributes local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void UpdateUserAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update User Read-Only Attributes
	 * Updates locally cached list of user read-only attributes.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of a user whose attributes should be updated.
	 * @param AttributeKeys Keys of the attributes that should be updated.
	 * @param SuccessCallback Callback function called after successful user attributes local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void UpdateUserReadOnlyAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

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
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Remove User Attributes
	 * Removes user attributes with specified keys (changes made on the server side).
	 *
	 * @param AuthToken User authorization token.
	 * @param AttributesToRemove List of attribute keys for removal.
	 * @param SuccessCallback Callback function called after successful user attributes removal on the server side.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Create Code for Linking Accounts
	 * Creates code for linking the user platform account to the main account.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful account linking code creation. The new linking code will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateAccountLinkingCode(const FString& AuthToken, const FOnCodeReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

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
	 * @param Platform Platform name.
	 * @param Code Account linking code obtained from the master account.
	 * @param SuccessCallback Callback function called after successful account linking.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkAccount(const FString& UserId, const EXsollaTargetPlatform Platform, const FString& Code,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Cross-Authenticate
	 * Authenticates a platform account user.
	 *
	 * @param UserId User identifier from a platform account.
	 * @param Platform Platform name.
	 * @param SuccessCallback Callback function called after successful user authentication on a specified platform.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticatePlatformAccountUser(const FString& UserId, const EXsollaTargetPlatform Platform, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Auth Via Access Token of Social Network
	* Authenticates the user with the access token using social network credentials.
	*
	* @param AuthToken Access token received from a social network
	* @param AuthTokenSecret Parameter 'oauth_token_secret' received from the authorization request. Required for Twitter only.
	* @param OpenId Parameter 'openid' received from the social network. Required for WeChat only.
	* @param ProviderName Name of the social network connected to Login in Publisher Account. Can have the following values: 'facebook', 'google', 'linkedin', 'twitter', 'discord', 'naver', and 'baidu'.
	* @param Payload Your custom data. The value of the parameter will be returned in the user JWT > payload claim (JWT only).
	* @param State Value used for additional user verification. Often used to mitigate CSRF Attacks. The value will be returned in the response. Must be longer than 8 symbols.
	* @param SuccessCallback Callback function called after successful user authentication on the specified platform.
	* @param ErrorCallback Callback function called after the request resulted with an error.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthViaAccessTokenOfSocialNetwork(const FString& AuthToken, const FString& AuthTokenSecret, const FString& OpenId,
		const FString& ProviderName, const FString& Payload, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update User Details
	 * Updates locally cached user details.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful user details local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateUserDetails(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

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
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update User Email
	 * Updates locally cached user email.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful user email local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateUserEmail(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update User Phone Number
	 * Updates locally cached user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful user phone number local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateUserPhoneNumber(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Modify User Phone Number
	 * Modifies user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber New user phone number.
	 * @param SuccessCallback Callback function called after user phone number was successfully modified.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Remove User Phone Number
	 * Removes the user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber User phone number for removal. If the parameter isn't specified, the locally cached phone number will be used instead.
	 * @param SuccessCallback Callback function called after the user phone number was successfully removed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Modify User Profile Picture
	 * Modifies user profile picture.
	 *
	 * @param AuthToken User authorization token.
	 * @param Picture New user profile picture.
	 * @param SuccessCallback Callback function called after the user profile picture was successfully modified.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserProfilePicture(const FString& AuthToken, UTexture2D* Picture, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Remove User Profile Picture
	 * Removes user profile picture.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after user profile picture was successfully removed.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveProfilePicture(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Friends
	 * Updates locally cached user friends data.
	 *
	 * @param AuthToken User authorization token.
	 * @param Type Friends type.
	 * @param SortBy Condition for sorting users (by name/by update).
	 * @param SortOrder Condition for sorting users (ascending/descending).
	 * @param SuccessCallback Callback function called after successful user friends data local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param After Parameter that is used for API pagination.
	 * @param Limit Maximum number of friends that can be received at a time.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateFriends(const FString& AuthToken, EXsollaFriendsType Type, EXsollaUsersSortCriteria SortBy, EXsollaUsersSortOrder SortOrder,
		const FOnUserFriendsUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, const FString& After, const int Limit = 20);

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
	void ModifyFriends(const FString& AuthToken, EXsollaFriendAction Action, const FString& UserID, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Social Authentication Links
	 * Updates locally cached list of links for social authentication enabled in Publisher Account.
	 *
	 * @param AuthToken User authorization token.
	 * @param Locale Locale.
	 * @param SuccessCallback Callback function called after locally cached list of links for social authentication was successfully updated.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateSocialAuthLinks(const FString& AuthToken, const FString& Locale, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Social Friends
	 * Updates locally cached user friends data from a social provider.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of social provider. If empty, friends from all available social providers will be fetched.
	 * @param SuccessCallback Callback function called after successful update of the user friends data from the social provider local cache.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of the element from which the list is generated.
	 * @param Limit Maximum number of friends that can be received at a time.
	 * @param FromThisGame Flag indicating whether social friends are from this game.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateSocialFriends(const FString& AuthToken, const FString& Platform,
		const FOnUserSocialFriendsUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, int Offset = 0, int Limit = 500, bool FromThisGame = false);

	/** Update Users Friends
	 * Updates friends on the server.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of the chosen social provider. If not specified, the method gets friends from all social providers.
	 * @param SuccessCallback Callback function called after successful update of user friends.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateUsersFriends(const FString& AuthToken, const FString& Platform, const FOnCodeReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Get Access Token By Email
	 * Get Access token by email from your own backend service.
	 *
	 * @param Parameters Parameters for custom auth.
	 * @param SuccessCallback Callback function called after the access token is successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Inventory|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetAccessTokenFromCustomAuthServer(const FXsollaParameters Parameters, const FOnAccessTokenLoginSuccess& SuccessCallback,
		const FOnAuthError& ErrorCallback);

	/** Get User Profile
	 * Gets specified user public profile information.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserID User identifier of public profile information to be received.
	 * @param SuccessCallback Callback function called after user profile data was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserProfile(const FString& AuthToken, const FString& UserID, const FOnUserProfileReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Search Users By Nickname
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
		const FOnUserSearchUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, int Offset = 0, int Limit = 100);

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
		const FOnSocialAccountLinkingHtmlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Linked Social Networks
	 * Updates the list of linked social networks cached locally.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after the list of linked social networks was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateLinkedSocialNetworks(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

protected:
	void RegisterUserJWT(const FString& Username, const FString& Password, const FString& Email, const FString& Payload,
		bool PersonalDataProcessingConsent, bool ReceiveNewsConsent, TArray<FString> AdditionalFields,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);
	void RegisterUserOAuth(const FString& Username, const FString& Password, const FString& Email, const FString& State,
		bool PersonalDataProcessingConsent, bool ReceiveNewsConsent, TArray<FString> AdditionalFields,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	void ResendAccountConfirmationEmailJWT(const FString& Username, const FString& Payload, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);
	void ResendAccountConfirmationEmailOAuth(const FString& Username, const FString& State, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	void AuthenticateUserJWT(const FString& Username, const FString& Password, const FString& Payload, bool bRememberMe, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);
	void AuthenticateUserOAuth(const FString& Username, const FString& Password, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	void GetSocialAuthenticationUrlJWT(const FString& ProviderName, const FString& Payload, const TArray<FString>& AdditionalFields,
		const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);
	void GetSocialAuthenticationUrlOAuth(const FString& ProviderName, const FString& State,
		const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	void AuthenticateWithSessionTicketJWT(const FString& ProviderName, const FString& AppId, const FString& SessionTicket, const FString& Code, const FString& Payload,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);
	void AuthenticateWithSessionTicketOAuth(const FString& ProviderName, const FString& AppId, const FString& SessionTicket, const FString& Code, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	void AuthViaAccessTokenOfSocialNetworkJWT(const FString& AuthToken, const FString& AuthTokenSecret, const FString& OpenId, const FString& ProviderName, const FString& Payload,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);
	void AuthViaAccessTokenOfSocialNetworkOAuth(const FString& AuthToken, const FString& AuthTokenSecret, const FString& OpenId, const FString& ProviderName, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	void Default_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserLogin_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void UserLoginOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void TokenVerify_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SocialAuthUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnSocialUrlReceived SuccessCallback, FOnAuthError ErrorCallback);
	void CrossAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void UpdateUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UpdateReadOnlyUserAttributes_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void AccountLinkingCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnCodeReceived SuccessCallback, FOnAuthError ErrorCallback);
	void CheckUserAge_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnCheckUserAgeSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void AuthConsoleAccountUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void RefreshTokenOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SessionTicketOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void AuthViaAccessTokenOfSocialNetworkJWT_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void AuthViaAccessTokenOfSocialNetworkOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void UserDetails_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void ModifyPhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void RemovePhoneNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserProfilePicture_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserProfilePictureRemove_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void UserFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnUserFriendsUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SocialAuthLinks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void SocialFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnUserSocialFriendsUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void UpdateUsersFriends_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnCodeReceived SuccessCallback, FOnAuthError ErrorCallback);
	void UserProfile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnUserProfileReceived SuccessCallback, FOnAuthError ErrorCallback);
	void UserSearch_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnUserSearchUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SocialAccountLinking_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnSocialAccountLinkingHtmlReceived SuccessCallback, FOnAuthError ErrorCallback);
	void LinkedSocialNetworks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);
	void GetAccessTokenByEmail_HttpRequestComplete(const FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse,
		const bool bSucceeded, FOnAccessTokenLoginSuccess SuccessCallback, FOnAuthError ErrorCallback);

	/** Processes the request for obtaining/refreshing token using OAuth 2.0. */
	void HandleOAuthTokenRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError& ErrorCallback, FOnAuthUpdate& SuccessCallback);

	/** Returns true if the error occurs. */
	void HandleRequestError(XsollaHttpRequestError ErrorData, FOnAuthError ErrorCallback);

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Url, const EXsollaHttpRequestVerb Verb = EXsollaHttpRequestVerb::VERB_GET,
		const FString& Content = FString(), const FString& AuthToken = FString());

	/** Encodes the request body to match x-www-form-urlencoded data format. */
	FString EncodeFormData(TSharedPtr<FJsonObject> FormDataJson);

	/** Sets a JSON string array field named FieldName and value of Array. */
	void SetStringArrayField(TSharedPtr<FJsonObject> Object, const FString& FieldName, const TArray<FString>& Array) const;

	/** Parses a JWT token and gets its payload as a JSON object. */
	bool ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject) const;

	/** Gets the name of a target platform. */
	FString GetTargetPlatformName(EXsollaTargetPlatform Platform);

	/** Cached Xsolla project ID. */
	FString ProjectID;

	/** Cached Xsolla Login project. */
	FString LoginID;

public:
	/** Gets user login state data. */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Login")
	FXsollaLoginData GetLoginData();

	/** Sets user login state data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetLoginData(const FXsollaLoginData& Data, bool ClearCache = true);

	/** Drops cache and cleans login data. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void DropLoginData(bool ClearCache = true);

	/** Gets user ID from the specified JWT token.
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetUserId(const FString& Token);

	/** Gets a token provider.
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetTokenProvider(const FString& Token);

	/** Get the Token Parameter
	 * Gets a value of the specified JWT token parameter.
	 *
	 * @param Token User authorization token.
	 * @param Parameter Name of parameter which value should be extracted from token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetTokenParameter(const FString& Token, const FString& Parameter);

	/** Checks if the specified JWT token represents the master account.
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	bool IsMasterAccount(const FString& Token);

	/** Loads save game and extract data. */
	void LoadSavedData();

	/** Saves cached data or resets it if RememberMe is false. */
	void SaveData();

	/** Gets the pending social authentication URL to be opened in the browser. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetPendingSocialAuthenticationUrl() const;

	/** Gets cached HTML page for social account linking. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetSocialAccountLinkingHtml() const;

	/** Gets user attributes. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaUserAttribute> GetUserAttributes();

	/** Gets user read-only attributes. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaUserAttribute> GetUserReadOnlyAttributes();

	/** Gets user details. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FXsollaUserDetails GetUserDetails() const;

	/** Gets user friends. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaSocialAuthLink> GetSocialAuthLinks() const;

	/** Gets user friends from social networks. Returns the list of users obtained during last UpdateSocialFriends method call. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FXsollaSocialFriendsData GetSocialFriends() const;

	/** Gets social profiles associated with specified user. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaSocialFriend> GetSocialProfiles(const FString& UserID) const;

	/** Gets linked social networks. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaLinkedSocialNetworkData> GetLinkedSocialNetworks() const;

	/** Checks if the specified social network is linked to user profile. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	bool IsSocialNetworkLinked(const FString& Provider) const;

protected:
	/** Keeps state of user login. */
	FXsollaLoginData LoginData;

	/** Social authentication url to be opened in browser. */
	FString PendingSocialAuthenticationUrl;

	/** Cached HTML page for social account linking. */
	FString SocialAccountLinkingHtml;

	/** Cached list of user attributes. */
	TArray<FXsollaUserAttribute> UserAttributes;

	/** Cached list of user read-only attributes. */
	TArray<FXsollaUserAttribute> UserReadOnlyAttributes;

	/** Cached user details. */
	FXsollaUserDetails UserDetails;

	/** Cached social auth links. */
	TArray<FXsollaSocialAuthLink> SocialAuthLinks;

	/** Cached list of user's social network friends that was obtained during last UpdateSocialFriends method call. */
	FXsollaSocialFriendsData SocialFriendsData;

	/** Cached list of linked social networks. */
	TArray<FXsollaLinkedSocialNetworkData> LinkedSocialNetworks;

protected:
	static const FString RegistrationEndpoint;
	static const FString LoginEndpoint;
	static const FString LoginSocialEndpoint;
	static const FString ResetPasswordEndpoint;
	static const FString ProxyRegistrationEndpoint;
	static const FString ProxyLoginEndpoint;
	static const FString ProxyResetPasswordEndpoint;
	static const FString ValidateTokenEndpoint;
	static const FString UserAttributesEndpoint;
	static const FString CrossAuthEndpoint;
	static const FString AccountLinkingCodeEndpoint;
	static const FString LoginEndpointOAuth;
	static const FString BlankRedirectEndpoint;
	static const FString UserDetailsEndpoint;
	static const FString UsersEndpoint;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;
};
