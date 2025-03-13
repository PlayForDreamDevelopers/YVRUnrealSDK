// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystemYvr.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemYvrTypesPrivate.h"

class FOnlineYvrFriend : public FOnlineFriend
{
private:

	TSharedRef<const FUniqueNetIdYvr> UserId;
	const FString DisplayName;
	FOnlineUserPresence Presence;

	const FString InviteToken;

public:

	FOnlineYvrFriend(const int ID, const FString& InDisplayName, YvrUserPresenceStatus FriendPresenceStatus, const FString& InInviteToken) :
		UserId(new FUniqueNetIdYvr(ID)),
		DisplayName(InDisplayName),
		InviteToken(InInviteToken)
	{
		Presence.bIsOnline = FriendPresenceStatus == YvrUserPresenceStatus_Online;
	}

	virtual TSharedRef<const FUniqueNetId> GetUserId() const override
	{
		return UserId;
	}

	virtual FString GetRealName() const override
	{
		return TEXT("");
	}

	virtual FString GetDisplayName(const FString& Platform = FString()) const override
	{
		return DisplayName;
	}

	virtual bool GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const override
	{
		return false;
	}

	virtual EInviteStatus::Type GetInviteStatus() const override
	{
		return EInviteStatus::Accepted;
	}

	virtual const FOnlineUserPresence& GetPresence() const override
	{
		return Presence;
	}

	FString GetInviteToken() const
	{
		return InviteToken;
	}
};

/**
*	IOnlineFriends - Interface class for Friends
*/
class FOnlineFriendsYvr : public IOnlineFriends
{
private:

	/** Reference to the owning subsystem */
	FOnlineSubsystemYvr& YvrSubsystem;

	/** All the friends for the player */
	TMap<uint64, TSharedRef<FOnlineFriend>> PlayerFriends;

	/** Invitable users to a room for the player */
	TMap<uint64, TSharedRef<FOnlineFriend>> InvitableUsers;

PACKAGE_SCOPE:

	/**
	* ** INTERNAL **
	* Called when we get the results back from the MessageQueue
	*/
	void OnQueryFriendsComplete(YvrMessageHandle Message, bool bIsError, int32 LocalUserNum, const FString& ListName, TMap<uint64, TSharedRef<FOnlineFriend>>& OutList, bool bAppendToExistingMap, const FOnReadFriendsListComplete& Delegate);

public:

	static const FString FriendsListInviteableUsers;

	/**
	* Constructor
	*
	* @param InSubsystem - A reference to the owning subsystem
	*/
	FOnlineFriendsYvr(FOnlineSubsystemYvr& InSubsystem);

	/**
	* Default destructor
	*/
	virtual ~FOnlineFriendsYvr() = default;

	// Begin IOnlineFriends interface
	virtual bool ReadFriendsList(int32 LocalUserNum, const FString& ListName, const FOnReadFriendsListComplete& Delegate = FOnReadFriendsListComplete()) override;
	virtual bool DeleteFriendsList(int32 LocalUserNum, const FString& ListName, const FOnDeleteFriendsListComplete& Delegate = FOnDeleteFriendsListComplete()) override;
	virtual bool SendInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnSendInviteComplete& Delegate = FOnSendInviteComplete()) override;
	virtual bool AcceptInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnAcceptInviteComplete& Delegate = FOnAcceptInviteComplete()) override;
	virtual bool RejectInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual void SetFriendAlias(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FString& Alias, const FOnSetFriendAliasComplete& Delegate = FOnSetFriendAliasComplete()) override;
	virtual void DeleteFriendAlias(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnDeleteFriendAliasComplete& Delegate = FOnDeleteFriendAliasComplete()) override;
	virtual bool DeleteFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool GetFriendsList(int32 LocalUserNum, const FString& ListName, TArray< TSharedRef<FOnlineFriend> >& OutFriends) override;
	virtual TSharedPtr<FOnlineFriend> GetFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool IsFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool QueryRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace) override;
	virtual bool GetRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace, TArray< TSharedRef<FOnlineRecentPlayer> >& OutRecentPlayers) override;
	virtual void DumpRecentPlayers() const override;
	virtual bool BlockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId) override;
	virtual bool UnblockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId) override;
	virtual bool QueryBlockedPlayers(const FUniqueNetId& UserId) override;
	virtual bool GetBlockedPlayers(const FUniqueNetId& UserId, TArray< TSharedRef<FOnlineBlockedPlayer> >& OutBlockedPlayers) override;
	virtual void DumpBlockedPlayers() const override;
	// End IOnlineFriends interface
};

