#include "OnlineFriendsYvr.h"

FOnlineFriendsYvr::FOnlineFriendsYvr(FOnlineSubsystemYvr& InSubsystem)
: YvrSubsystem(InSubsystem)
{
}

const FString FOnlineFriendsYvr::FriendsListInviteableUsers = TEXT("invitableUsers");

bool FOnlineFriendsYvr::ReadFriendsList(int32 LocalUserNum, const FString& ListName, const FOnReadFriendsListComplete& Delegate)
{
#if PLATFORM_ANDROID
	if (ListName == EFriendsLists::ToString(EFriendsLists::Default) || ListName == EFriendsLists::ToString(EFriendsLists::OnlinePlayers))
	{
		YvrSubsystem.AddRequestDelegate(
			OnlineSubsystemYvrWrapper::GetYvrFriends(),
			FYvrMessageOnCompleteDelegate::CreateLambda([this, LocalUserNum, ListName, Delegate](YvrMessageHandle Message, bool bIsError)
				{
					OnQueryFriendsComplete(Message, bIsError, LocalUserNum, ListName, PlayerFriends, false, Delegate);
				}));
		return true;
	}

	if (ListName == FriendsListInviteableUsers)
	{
		//todo
		return true;
	}

	Delegate.ExecuteIfBound(LocalUserNum, false, ListName, TEXT("Invalid friends list"));
#endif
	return false;
}

void FOnlineFriendsYvr::OnQueryFriendsComplete(YvrMessageHandle Message, bool bIsError, int32 LocalUserNum, const FString& ListName, TMap<uint64, TSharedRef<FOnlineFriend>>& OutList, bool bAppendToExistingMap, const FOnReadFriendsListComplete& Delegate)
{
#if PLATFORM_ANDROID
	FString ErrorStr;
	if (bIsError)
	{
		auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
		ErrorStr = FString(ErrorMessage);
		Delegate.ExecuteIfBound(LocalUserNum, false, ListName, ErrorStr);
		return;
	}

	auto UserNum = OnlineSubsystemYvrWrapper::GetFriendsSize(Message);

	if (!bAppendToExistingMap)
	{
		OutList.Empty(UserNum);
	}

	for (size_t FriendIndex = 0; FriendIndex < UserNum; ++FriendIndex)
	{
		auto Friend = OnlineSubsystemYvrWrapper::GetItemOfFriendsList(Message, FriendIndex);
		auto FriendId = OnlineSubsystemYvrWrapper::GetActIdOfFriendItem(Friend);
		auto FriendDisplayName = OnlineSubsystemYvrWrapper::GetNickOfFriendItem(Friend);
		auto FriendPresenceStatus = static_cast<YvrUserPresenceStatus>(OnlineSubsystemYvrWrapper::GetOnlineOfFriendItem(Friend));
		// todo: invite token
		FString FriendInviteTokenString = "";

		TSharedRef<FOnlineYvrFriend> OnlineFriend(new FOnlineYvrFriend(FriendId, FriendDisplayName, FriendPresenceStatus, FriendInviteTokenString));

		OutList.Add(FriendId, OnlineFriend);
	}

	// todo: paging
	Delegate.ExecuteIfBound(LocalUserNum, true, ListName, ErrorStr);
#endif
}

bool FOnlineFriendsYvr::DeleteFriendsList(int32 LocalUserNum, const FString& ListName, const FOnDeleteFriendsListComplete& Delegate)
{
	return false;
}

bool FOnlineFriendsYvr::SendInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnSendInviteComplete& Delegate)
{
	return false;
}

bool FOnlineFriendsYvr::AcceptInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnAcceptInviteComplete& Delegate)
{
	return false;
}

bool FOnlineFriendsYvr::RejectInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName)
{
	return false;
}

void FOnlineFriendsYvr::SetFriendAlias(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FString& Alias, const FOnSetFriendAliasComplete& Delegate)
{
}

void FOnlineFriendsYvr::DeleteFriendAlias(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnDeleteFriendAliasComplete& Delegate)
{
}

bool FOnlineFriendsYvr::DeleteFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName)
{
	return false;
}

bool FOnlineFriendsYvr::GetFriendsList(int32 LocalUserNum, const FString& ListName, TArray<TSharedRef<FOnlineFriend>>& OutFriends)
{
	if (ListName == EFriendsLists::ToString(EFriendsLists::Default))
	{
		PlayerFriends.GenerateValueArray(OutFriends);
		return true;
	}
	if (ListName == EFriendsLists::ToString(EFriendsLists::OnlinePlayers))
	{
		for (auto Friend : PlayerFriends)
		{
			if (Friend.Value->GetPresence().bIsOnline)
			{
				OutFriends.Add(Friend.Value);
			}
		}
		return true;
	}
	if (ListName == FriendsListInviteableUsers)
	{
		InvitableUsers.GenerateValueArray(OutFriends);
		return true;
	}

	return false;
}

TSharedPtr<FOnlineFriend> FOnlineFriendsYvr::GetFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName)
{
	auto YvrFriendId = static_cast<const FUniqueNetIdYvr&>(FriendId);

	if (ListName == EFriendsLists::ToString(EFriendsLists::Default))
	{
		if (!PlayerFriends.Contains(YvrFriendId.GetID()))
		{
			return nullptr;
		}
		return PlayerFriends[YvrFriendId.GetID()];
	}

	if (ListName == EFriendsLists::ToString(EFriendsLists::OnlinePlayers))
	{
		if (!PlayerFriends.Contains(YvrFriendId.GetID()))
		{
			return nullptr;
		}

		auto Friend = PlayerFriends[YvrFriendId.GetID()];
		if (!Friend->GetPresence().bIsOnline)
		{
			return nullptr;
		}

		return Friend;
	}

	if (ListName == FriendsListInviteableUsers)
	{
		if (!InvitableUsers.Contains(YvrFriendId.GetID()))
		{
			return nullptr;
		}
		return InvitableUsers[YvrFriendId.GetID()];
	}

	return nullptr;
}

bool FOnlineFriendsYvr::IsFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName)
{
	auto Friend = GetFriend(0, FriendId, ListName);
	return Friend.IsValid();
}

bool FOnlineFriendsYvr::QueryRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace)
{
	return false;
}

bool FOnlineFriendsYvr::GetRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace, TArray<TSharedRef<FOnlineRecentPlayer>>& OutRecentPlayers)
{
	return false;
}

void FOnlineFriendsYvr::DumpRecentPlayers() const
{
}

bool FOnlineFriendsYvr::BlockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId)
{
	return false;
}

bool FOnlineFriendsYvr::UnblockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId)
{
	return false;
}

bool FOnlineFriendsYvr::QueryBlockedPlayers(const FUniqueNetId& UserId)
{
	return false;
}

bool FOnlineFriendsYvr::GetBlockedPlayers(const FUniqueNetId& UserId, TArray<TSharedRef<FOnlineBlockedPlayer>>& OutBlockedPlayers)
{
	return false;
}

void FOnlineFriendsYvr::DumpBlockedPlayers() const
{
}
