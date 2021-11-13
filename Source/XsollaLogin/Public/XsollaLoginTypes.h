// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaLoginTypes.generated.h"

UENUM(BlueprintType)
enum class EXsollaFriendsType : uint8
{
	/** Users added to a friends list. */
	friends UMETA(DisplayName = "Friends"),
	/** Users to whom a friend request was sent. */
	friend_requested UMETA(DisplayName = "FriendsRequested"),
	/** Users from whom a friend request was sent. */
	friend_requested_by UMETA(DisplayName = "FriendsRequestedBy"),
	/** Users added to a friend list but blocked. */
	blocked UMETA(DisplayName = "Blocked"),
	/** Users who blocked this user. */
	blocked_by UMETA(DisplayName = "BlockedBy")
};

UENUM(BlueprintType)
enum class EXsollaSessionType : uint8
{
	/** all is used for deleting the SSO user session and invalidating all access and refresh tokens. */
	all UMETA(DisplayName = "All"),
	/** sso is used for deleting only the SSO user session. */
	sso UMETA(DisplayName = "SSO")
};

UENUM(BlueprintType)
enum class EXsollaUsersSortCriteria : uint8
{
	/** Sorts the list of users by nickname alphabetically. */
	by_nickname UMETA(DisplayName = "ByNickname"),
	/** Sorts the list of users by the date when they last added someone to their friends list or blocked them. */
	by_update UMETA(DisplayName = "ByUpdate")
};

UENUM(BlueprintType)
enum class EXsollaUsersSortOrder : uint8
{
	asc UMETA(DisplayName = "Ascending"),
	desc UMETA(DisplayName = "Descending")
};

UENUM(BlueprintType)
enum class EXsollaFriendAction : uint8
{
	friend_request_add UMETA(DisplayName = "RequestAdd"),
	friend_request_cancel UMETA(DisplayName = "RequestCancel"),
	friend_request_approve UMETA(DisplayName = "RequestApprove"),
	friend_request_deny UMETA(DisplayName = "RequestDeny"),
	friend_remove UMETA(DisplayName = "Remove"),
	block UMETA(DisplayName = "Block"),
	unblock UMETA(DisplayName = "Unblock")
};

USTRUCT(BlueprintType)
struct FXsollaAuthToken
{
	GENERATED_BODY()

	/** A JWT signed by the secret key is generated for each successfully authenticated user. */
	UPROPERTY(BlueprintReadOnly, Category = "Auth Token")
	FString JWT;

	/** Token expiration time. */
	UPROPERTY(BlueprintReadOnly, Category = "Auth Token")
	int64 ExpiresAt;

	/** Token verification status. */
	UPROPERTY(BlueprintReadOnly, Category = "Auth Token")
	bool bIsVerified;

	/** Refreshes token for updating the access token (JWT). */
	UPROPERTY(BlueprintReadOnly, Category = "Auth Token")
	FString RefreshToken;

	FXsollaAuthToken()
		: ExpiresAt(0)
		, bIsVerified(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaLoginData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	FXsollaAuthToken AuthToken;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	FString Username;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	FString Password;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	bool bRememberMe;

	UPROPERTY(BlueprintReadOnly, Category = "Login Data")
	bool bEncrypted;

	FXsollaLoginData()
		: bRememberMe(false)
		, bEncrypted(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaUserAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Attribute")
	FString key;

	UPROPERTY(BlueprintReadWrite, Category = "User Attribute")
	FString permission;

	UPROPERTY(BlueprintReadWrite, Category = "User Attribute")
	FString value;
};

USTRUCT(BlueprintType)
struct FXsollaUserBan
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Ban")
	FString date_from;

	UPROPERTY(BlueprintReadWrite, Category = "User Ban")
	FString date_to;

	UPROPERTY(BlueprintReadWrite, Category = "User Ban")
	FString reason;
};

USTRUCT(BlueprintType)
struct FXsollaUserGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Group")
	int32 id;

	UPROPERTY(BlueprintReadWrite, Category = "User Group")
	bool is_default;

	UPROPERTY(BlueprintReadWrite, Category = "User Group")
	bool is_deletable;

	UPROPERTY(BlueprintReadWrite, Category = "User Group")
	FString name;

	FXsollaUserGroup()
		: id(0)
		, is_default(false)
		, is_deletable(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaUserDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FXsollaUserBan ban;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString birthday;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString connection_information;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString country;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString email;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString external_id;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString first_name;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString gender;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	TArray<FXsollaUserGroup> groups;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString id;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	bool is_anonymous;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString last_login;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString last_name;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString name;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString nickname;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString tag;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString phone;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString picture;

	UPROPERTY(BlueprintReadWrite, Category = "User Details")
	FString registered;

	UPROPERTY(BlueprintReadWrite, Category = "Friend Details")
	FString presence;

	FXsollaUserDetails()
		: is_anonymous(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaFriendDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Friend Details")
	FString status_incoming;

	UPROPERTY(BlueprintReadWrite, Category = "Friend Details")
	FString status_outgoing;

	UPROPERTY(BlueprintReadWrite, Category = "Friend Details")
	int64 updated;

	UPROPERTY(BlueprintReadWrite, Category = "Friend Details")
	FXsollaUserDetails user;

	FXsollaFriendDetails()
		: updated(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaFriendsData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Friends Data")
	FString next_after;

	UPROPERTY(BlueprintReadWrite, Category = "Friends Data")
	FString next_url;

	UPROPERTY(BlueprintReadWrite, Category = "Friends Data")
	TArray<FXsollaFriendDetails> relationships;
};

USTRUCT(BlueprintType)
struct FXsollaSocialAuthLink
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Social Authentication")
	FString auth_url;

	UPROPERTY(BlueprintReadWrite, Category = "Social Authentication")
	FString provider;
};

USTRUCT(BlueprintType)
struct FXsollaSocialFriend
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Social Friend")
	FString avatar;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friend")
	FString name;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friend")
	FString platform;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friend")
	FString tag;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friend")
	FString user_id;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friend")
	FString xl_uid;
};

USTRUCT(BlueprintType)
struct FXsollaSocialFriendsData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Social Friends Data")
	TArray<FXsollaSocialFriend> data;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friends Data")
	int32 limit;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friends Data")
	int32 offset;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friends Data")
	FString platform;

	UPROPERTY(BlueprintReadWrite, Category = "Social Friends Data")
	int32 total_count;

	FXsollaSocialFriendsData()
		: limit(0)
		, offset(0)
		, total_count(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaPublicProfile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	FString avatar;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	bool is_me;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	FString last_login;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	FString nickname;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	FString tag;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	FString registered;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	FString user_id;

	FXsollaPublicProfile()
		: is_me(false)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaUserSearchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	int32 offset;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	int32 total_count;

	UPROPERTY(BlueprintReadWrite, Category = "User Public Profile")
	TArray<FXsollaPublicProfile> users;

	FXsollaUserSearchResult()
		: offset(0)
		, total_count(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FXsollaLinkedSocialNetworkData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString full_name;

	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString nickname;

	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString picture;

	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString provider;

	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString social_id;
};

USTRUCT(BlueprintType)
struct FXsollaCheckUserAgeResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Check User Age")
	bool accepted;
};

USTRUCT(BlueprintType)
struct FXsollaProviderToken
{
	GENERATED_BODY()

	/** User JWT for the client project. */
	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString access_token;

	/** 'access_token' expiration period in seconds. You can configure it in Publisher Account > your Login project > General Settings > JWT expiration. */
	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	int64 expires_in;

	/** Refresh token for updating the 'access token'. */
	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString refresh_token;

	/** JWT type. Can be 'bearer'. */
	UPROPERTY(BlueprintReadWrite, Category = "Social Network")
	FString token_type;
};

USTRUCT(BlueprintType)
struct FXsollaUserDevice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "User Device")
	FString device;

	UPROPERTY(BlueprintReadWrite, Category = "User Device")
	int64 id;

	UPROPERTY(BlueprintReadWrite, Category = "User Device")
	FString last_used_at;

	UPROPERTY(BlueprintReadWrite, Category = "User Device")
	FString type;
};