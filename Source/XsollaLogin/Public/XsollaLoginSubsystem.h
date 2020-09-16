// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "XsollaLoginTypes.h"

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

#include "XsollaLoginSubsystem.generated.h"

/** Verb (GET, PUT, POST) used by the request */
UENUM(BlueprintType)
enum class EXsollaLoginRequestVerb : uint8
{
	GET,
	POST,
	PUT,
	DELETE,
	PATCH
};

class FJsonObject;

/** Common callback for operations without any user-friendly messages from server on success */
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

	/** Initialize controller with provided Project ID and Login ID (use to override project settings)
	 *
	 * @param InProjectId New Project ID value from Publisher Account Project settings > Project ID.
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
	 * @param SuccessCallback Callback function called after successful user registration. Account confirmation message will be send to the specified email.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RegisterUser(const FString& Username, const FString& Password, const FString& Email, const FString& State,
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
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, bool bRememberMe = false);

	/** Reset User Password
	 * Resets the user's password.
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
	 * Gets URL for authentication via specified social network.
	 *
	 * @param ProviderName Name of a social network. Provider must be connected to Login in Publisher Account. Required.
	 * @param SuccessCallback Callback function called after URL for social authentication was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetSocialAuthenticationUrl(const FString& ProviderName, const FString& State, const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Launch social authentication
	 * Opens social authentication URL in browser.
	 *
	 * @param SocialAuthenticationUrl URL with social network authentication form.
	 * @param BrowserWidget Widget to represent social network authentication form. Can be set in project settings.
	 * @param bRememberMe Whether the user agrees to save the authentication data. Default is 'false'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void LaunchSocialAuthentication(const FString& SocialAuthenticationUrl, UUserWidget*& BrowserWidget, bool bRememberMe = false);

	/** Set a new value of token (used when token obtained via social network authentication etc.)
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void SetToken(const FString& Token);

	/** Refresh the token in case it is expired. Works only when OAuth 2.0 is enabled.
	 *
	 * @param RefreshToken Token used to refresh the expired access token. Received when authorizing user with username/password for the firs time.
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
	 * Authenticates a user by exchanging platform specific session ticket to token.
	 *
	 * @param ProviderName Platform on which session ticket was obtained.
	 * @param SessionTicket Session ticket.
	 * @param AppId Platform application identifier.
	 * @param State Value used for additional user verification. Required for OAuth 2.0.
	 * @param SuccessCallback Callback function called after successful user authentication with a platform session ticket. Authentication data including a JWT will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void AuthenticateWithSessionTicket(const FString& ProviderName, const FString& SessionTicket, const FString& AppId, const FString& State,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update User Attributes
	 * Updates locally cached list of user attributes.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserId Identifier of user which attributes should be updated for.
	 * @param AttributeKeys Keys of attributes that should be updated.
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
	 * @param UserId Identifier of user which attributes should be updated for.
	 * @param AttributeKeys Keys of attributes that should be updated.
	 * @param SuccessCallback Callback function called after successful user attributes local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "AttributeKeys, SuccessCallback, ErrorCallback"))
	void UpdateUserReadOnlyAttributes(const FString& AuthToken, const FString& UserId, const TArray<FString>& AttributeKeys,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Modify User Attributes
	 * Modifies the list of user attributes by creating/editing its items (changes made on server side).
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
	 * Removes user attributes with specified keys (changes made on server side).
	 *
	 * @param AuthToken User authorization token.
	 * @param AttributesToRemove List of attribute keys to be removed.
	 * @param SuccessCallback Callback function called after successful user attributes removal on the server side.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserAttributes(const FString& AuthToken, const TArray<FString>& AttributesToRemove, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Create Code for Linking Accounts
	 * Creates code for linking user platform account to the main account.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after successful accout linking code creation. New linking code will be received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CreateAccountLinkingCode(const FString& AuthToken, const FOnCodeReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Link Accounts by Code
	 * Links the user platform account to the existing main account by the code.
	 *
	 * @param UserId User identifier from a platform account.
	 * @param Platform Platform name.
	 * @param Code Account linking code obtained from master account.
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
	 * @param SuccessCallback Callback function called after succesfull user authentication on specified platform.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void AuthenticatePlatformAccountUser(const FString& UserId, const EXsollaTargetPlatform Platform, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

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
	 * @param Birthday User  birth date in format (YYYY-MM-DD). Can be changed only once.
	 * @param FirstName User first name. Pass empty string to remove current first name.
	 * @param LastName User last name. Pass empty string to remove current last name.
	 * @param Gender User gender (f - for female, m - for male).
	 * @param Nickname User nickname. Pass empty string to remove current nickname.
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
	 * @param SuccessCallback Callback function called after user phone number modified successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Remove User Phone Number
	 * Removes user phone number.
	 *
	 * @param AuthToken User authorization token.
	 * @param PhoneNumber User phone number to be removed. If parameter is not specified, locally cached phone  number will be used instead.
	 * @param SuccessCallback Callback function called after user phone number removed successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveUserPhoneNumber(const FString& AuthToken, const FString& PhoneNumber, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Modify User Profile Picture
	 * Modifies user profile picture.
	 *
	 * @param AuthToken User authorization token.
	 * @param Picture New user profile picture.
	 * @param SuccessCallback Callback function called after user profile picture modified successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ModifyUserProfilePicture(const FString& AuthToken, UTexture2D* Picture, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Remove User Profile Picture
	 * Removes user profile picture.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after user profile picture removed successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void RemoveProfilePicture(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Friends
	 * Updates locally cached user friends data.
	 *
	 * @param AuthToken User authorization token.
	 * @param Type Friends type.
	 * @param SortBy Condition for sorting users (by name / by update).
	 * @param SortOrder Condition for sorting users (ascending / descending).
	 * @param SuccessCallback Callback function called after successful user friends data local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateFriends(const FString& AuthToken, EXsollaFriendsType Type, EXsollaUsersSortCriteria SortBy, EXsollaUsersSortOrder SortOrder,
		const FOnUserFriendsUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Modify Friends
	 * Modifies relationships with specified user.
	 *
	 * @param AuthToken User authorization token.
	 * @param Action Type of action to be applied to a specified friend.
	 * @param UserID Identifier of user to change relationships with.
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
	 * @param SuccessCallback Callback function called after social auth links list local cache updated successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateSocialAuthLinks(const FString& AuthToken, const FString& Locale, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Social Friends
	 * Updates locally cached user friends data froma a social provider.
	 *
	 * @param AuthToken User authorization token.
	 * @param Platform Name of social provider. If empty friends from all available social providers will be fetched.
	 * @param SuccessCallback Callback function called after successful user friends data from social provider local cache update.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of elements from which list is generated.
	 * @param Limit Maximum number of friends that can be received at a time.
	 * @param FromThisGame Flag indicating whether social friends are from this game.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateSocialFriends(const FString& AuthToken, const FString& Platform,
		const FOnUserSocialFriendsUpdate& SuccessCallback, const FOnAuthError& ErrorCallback, int Offset = 0, int Limit = 500, bool FromThisGame = false);

	/** Get User Profile
	 * Gets specified user public profile information.
	 *
	 * @param AuthToken User authorization token.
	 * @param UserID User identifier of public profile information to be received.
	 * @param SuccessCallback Callback function called after user profile datareceived successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetUserProfile(const FString& AuthToken, const FString& UserID, const FOnUserProfileReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Search Users By Nickname
	 * Search for users with specified nickname.
	 *
	 * @param Nickname User nickname used as search criteria.
	 * @param SuccessCallback Callback function called after user search completed successfully.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 * @param Offset Number of elements from which list is generated.
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
	 * @param SuccessCallback Callback function called after URL for social authentication was successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LinkSocialNetworkToUserAccount(const FString& AuthToken, const FString& ProviderName,
		const FOnSocialAccountLinkingHtmlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	/** Update Linked Social Networks
	 * Update list of linked social networks cached locally.
	 *
	 * @param AuthToken User authorization token.
	 * @param SuccessCallback Callback function called after list of linked social networks successfully received.
	 * @param ErrorCallback Callback function called after the request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void UpdateLinkedSocialNetworks(const FString& AuthToken, const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

protected:
	void RegisterUserJWT(const FString& Username, const FString& Password, const FString& Email,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);
	void RegisterUserOAuth(const FString& Username, const FString& Password, const FString& Email, const FString& State,
		const FOnRequestSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	void AuthenticateUserJWT(const FString& Username, const FString& Password, bool bRemeberMe, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);
	void AuthenticateUserOAuth(const FString& Username, const FString& Password, const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);

	void GetSocialAuthenticationUrlJWT(const FString& ProviderName, const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);
	void GetSocialAuthenticationUrlOAuth(const FString& ProviderName, const FString& State, const FOnSocialUrlReceived& SuccessCallback, const FOnAuthError& ErrorCallback);

	void AuthenticateWithSessionTicketJWT(const FString& ProviderName, const FString& AppId, const FString& SessionTicket,
		const FOnAuthUpdate& SuccessCallback, const FOnAuthError& ErrorCallback);
	void AuthenticateWithSessionTicketOAuth(const FString& ProviderName, const FString& AppId, const FString& SessionTicket, const FString& State,
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
	void AuthConsoleAccountUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void RefreshTokenOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnAuthUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SessionTicketOAuth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
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
	void UserProfile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnUserProfileReceived SuccessCallback, FOnAuthError ErrorCallback);
	void UserSearch_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnUserSearchUpdate SuccessCallback, FOnAuthError ErrorCallback);
	void SocialAccountLinking_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnSocialAccountLinkingHtmlReceived SuccessCallback, FOnAuthError ErrorCallback);
	void LinkedSocialNetworks_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
		FOnRequestSuccess SuccessCallback, FOnAuthError ErrorCallback);

	/** Process request for obtaining/refreshing token using OAuth 2.0 */
	void HandleOAuthTokenRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError& ErrorCallback, FOnAuthUpdate& SuccessCallback);

	/** Return true if error has happened */
	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnAuthError ErrorCallback);

private:
	/** Create http request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url, const EXsollaLoginRequestVerb Verb = EXsollaLoginRequestVerb::GET,
		const FString& Content = FString(), const FString& AuthToken = FString());

	/** Encode request body to match x-www-form-urlencoded data format */
	FString EncodeFormData(TSharedPtr<FJsonObject> FormDataJson);

	/** Set a Json string array field named FieldName and value of Array */
	void SetStringArrayField(TSharedPtr<FJsonObject> Object, const FString& FieldName, const TArray<FString>& Array) const;

	/** Parse JWT token and get its payload as json object */
	bool ParseTokenPayload(const FString& Token, TSharedPtr<FJsonObject>& PayloadJsonObject) const;

	/** Get name of target platform */
	FString GetTargetPlatformName(EXsollaTargetPlatform Platform);

	/** Cached Xsolla project id */
	FString ProjectID;

	/** Cached Xsolla Login project id */
	FString LoginID;

public:
	/** Get user login state data */
	UFUNCTION(BlueprintPure, Category = "Xsolla|Login")
	FXsollaLoginData GetLoginData();

	/** Drop cache and cleanup login data */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	void DropLoginData(bool ClearCache = true);

	/** Get user ID from the specified JWT token
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetUserId(const FString& Token);

	/** Get a token provider
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

	/** Check if the specified JWT token represents the master account
	 *
	 * @param Token User authorization token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	bool IsMasterAccount(const FString& Token);

	/** Load save game and extract data */
	void LoadSavedData();

	/** Save cached data or reset one if RememberMe is false */
	void SaveData();

	/** Get the pending social authentication URL to be opened in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetPendingSocialAuthenticationUrl() const;

	/** Get cached HTML page for social account linking */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FString GetSocialAccountLinkingHtml() const;

	/** Get user attributes */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaUserAttribute> GetUserAttributes();

	/** Get user read-only attributes */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaUserAttribute> GetUserReadOnlyAttributes();

	/** Get user details */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FXsollaUserDetails GetUserDetails() const;

	/** Get user friends */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaSocialAuthLink> GetSocialAuthLinks() const;

	/** Get user friends from social networks. Returns list of users obtained during last UpdateSocialFriends method call */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	FXsollaSocialFriendsData GetSocialFriends() const;

	/** Get linked social networks */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Login")
	TArray<FXsollaLinkedSocialNetworkData> GetLinkedSocialNetworks() const;

protected:
	/** Keeps state of user login */
	FXsollaLoginData LoginData;

	/** Social authentication url to be opened in browser */
	FString PendingSocialAuthenticationUrl;

	/** Cached HTML page for social account linking */
	FString SocialAccountLinkingHtml;

	/** Cached list of user attributes */
	TArray<FXsollaUserAttribute> UserAttributes;

	/** Cached list of user read-only attributes */
	TArray<FXsollaUserAttribute> UserReadOnlyAttributes;

	/** Cached user details */
	FXsollaUserDetails UserDetails;

	/** Cached social auth links */
	TArray<FXsollaSocialAuthLink> SocialAuthLinks;

	/** Cached list of user's social network friends that was obtained during last UpdateSocialFriends method call */
	FXsollaSocialFriendsData SocialFriendsData;

	/** Cached list of linked social networks */
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
