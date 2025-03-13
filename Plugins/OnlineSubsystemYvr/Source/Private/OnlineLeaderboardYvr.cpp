// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#include "OnlineLeaderboardYvr.h"

FOnlineLeaderboardYvr::FOnlineLeaderboardYvr(class FOnlineSubsystemYvr& InSubsystem)
: YvrSubsystem(InSubsystem)
{
}

bool FOnlineLeaderboardYvr::ReadLeaderboards(const TArray< TSharedRef<const FUniqueNetId> >& Players, FOnlineLeaderboardReadRef& ReadObject)
{
	bool bOnlyLoggedInUser = false;
	if (Players.Num() > 0)
	{
		auto LoggedInPlayerId = YvrSubsystem.GetIdentityInterface()->GetUniquePlayerId(0);
		if (Players.Num() == 1 && LoggedInPlayerId.IsValid() && *Players[0] == *LoggedInPlayerId)
		{
			bOnlyLoggedInUser = true;
		}
		else
		{
			UE_LOG_ONLINE_LEADERBOARD(Warning, TEXT("Filtering by player ids other than the logged in player is not supported.  Ignoring the 'Players' parameter"));
		}
	}
	return ReadYvrLeaderboards(false, bOnlyLoggedInUser, ReadObject);
};

bool FOnlineLeaderboardYvr::ReadLeaderboardsForFriends(int32 LocalUserNum, FOnlineLeaderboardReadRef& ReadObject)
{
	return ReadYvrLeaderboards(true, false, ReadObject);
}

bool FOnlineLeaderboardYvr::ReadLeaderboardsAroundRank(int32 Rank, uint32 Range, FOnlineLeaderboardReadRef& ReadObject)
{
	// UNDONE
	return false;
}

bool FOnlineLeaderboardYvr::ReadLeaderboardsAroundUser(TSharedRef<const FUniqueNetId> Player, uint32 Range, FOnlineLeaderboardReadRef& ReadObject)
{
	// UNDONE
	return false;
}

bool FOnlineLeaderboardYvr::ReadYvrLeaderboards(bool bOnlyFriends, bool bOnlyLoggedInUser, FOnlineLeaderboardReadRef& ReadObject)
{
#if PLATFORM_ANDROID
	auto FilterType = (bOnlyFriends) ? "friends" : "none";
	auto Limit = 100;
	auto StartAt = "none";

	// If only getting the logged in user, then only return back one result
	if (bOnlyLoggedInUser)
	{
		Limit = 1;
		StartAt = "aroundOnView";
	}

	ReadObject->ReadState = EOnlineAsyncTaskState::InProgress;
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::ReadLeaderboardItems(TCHAR_TO_ANSI(*ReadObject->LeaderboardName.ToString()), Limit, FilterType, StartAt),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, ReadObject](YvrMessageHandle Message, bool bIsError)
	{
		OnReadLeaderboardsComplete(Message, bIsError, ReadObject);
	}));
	return true;
#endif
	return false;
}

void FOnlineLeaderboardYvr::OnReadLeaderboardsComplete(YvrMessageHandle Message, bool bIsError, const FOnlineLeaderboardReadRef& ReadObject)
{
#if PLATFORM_ANDROID
	if (bIsError)
	{
		ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
		TriggerOnLeaderboardReadCompleteDelegates(false);
		return;
	}
	auto LeaderboardList = OnlineSubsystemYvrWrapper::GetLeaderboardList(Message);
	auto LeaderboardItemSize = OnlineSubsystemYvrWrapper::GetLeaderboardItemSize(LeaderboardList);

	for (size_t i = 0; i < LeaderboardItemSize; i++)
	{
		auto LeaderboardItem = OnlineSubsystemYvrWrapper::GetItemOfLeaderboard(LeaderboardList, i);

		auto NickName = OnlineSubsystemYvrWrapper::GetLeaderboardUserNickName(LeaderboardItem);
		auto UserID = OnlineSubsystemYvrWrapper::GetLeaderboardUserId(LeaderboardItem);
		auto Rank = OnlineSubsystemYvrWrapper::GetLeaderboardRank(LeaderboardItem);
		auto Score = OnlineSubsystemYvrWrapper::GetLeaderboardScore(LeaderboardItem);

		auto Row = FOnlineStatsRow(NickName, MakeShareable(new FUniqueNetIdYvr(UserID)));
		Row.Rank = Rank;
		Row.Columns.Add(ReadObject->SortedColumn, Score);
		ReadObject->Rows.Add(Row);
	}

	ReadObject->ReadState = EOnlineAsyncTaskState::Done;
	TriggerOnLeaderboardReadCompleteDelegates(true);
#endif
}

void FOnlineLeaderboardYvr::FreeStats(FOnlineLeaderboardRead& ReadObject)
{
	// no-op
}

bool FOnlineLeaderboardYvr::WriteLeaderboards(const FName& SessionName, const FUniqueNetId& Player, FOnlineLeaderboardWrite& WriteObject)
{
#if PLATFORM_ANDROID
	auto LoggedInPlayerId = YvrSubsystem.GetIdentityInterface()->GetUniquePlayerId(0);
	if (!(LoggedInPlayerId.IsValid() && Player == *LoggedInPlayerId))
	{
		UE_LOG_ONLINE_LEADERBOARD(Error, TEXT("Can only write to leaderboards for logged in player id"));
		return false;
	}

	auto StatData = WriteObject.FindStatByName(WriteObject.RatedStat);

	if (StatData == nullptr)
	{
		UE_LOG_ONLINE_LEADERBOARD(Error, TEXT("Could not find RatedStat: %s"), *WriteObject.RatedStat.ToString());
		return false;
	}

	float Score;
	StatData->GetValue(Score);

	for (const auto& LeaderboardName : WriteObject.LeaderboardNames)
	{
		YvrSubsystem.AddRequestDelegate(
			OnlineSubsystemYvrWrapper::WriteLeaderboardItem(TCHAR_TO_ANSI(*LeaderboardName.ToString()), Score, nullptr, 0, (WriteObject.UpdateMethod != ELeaderboardUpdateMethod::Force)),
			FYvrMessageOnCompleteDelegate::CreateLambda([this](YvrMessageHandle Message, bool bIsError)
		{
			if (bIsError)
			{
				auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
				UE_LOG_ONLINE_LEADERBOARD(Error, TEXT("%s"), *FString(ErrorMessage));
			}
		}));
	}

	return true;
#endif
	return false;
};

bool FOnlineLeaderboardYvr::FlushLeaderboards(const FName& SessionName)
{
	TriggerOnLeaderboardFlushCompleteDelegates(SessionName, true);
	return true;
};

bool FOnlineLeaderboardYvr::WriteOnlinePlayerRatings(const FName& SessionName, int32 LeaderboardId, const TArray<FOnlinePlayerScore>& PlayerScores)
{
	// Not supported
	return false;
};
