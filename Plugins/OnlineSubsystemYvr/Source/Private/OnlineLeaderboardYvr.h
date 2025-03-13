// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineIdentityYvr.h"
#include "OnlineSubsystemYvrWrapper.h"

class FOnlineLeaderboardYvr : public IOnlineLeaderboards
{
private:

	FOnlineSubsystemYvr& YvrSubsystem;

	bool ReadYvrLeaderboards(bool bOnlyFriends, bool bOnlyLoggedInUser, FOnlineLeaderboardReadRef& ReadObject);
	void OnReadLeaderboardsComplete(YvrMessageHandle Message, bool bIsError, const FOnlineLeaderboardReadRef& ReadObject);

public:

	FOnlineLeaderboardYvr(FOnlineSubsystemYvr& InSubsystem);

	virtual ~FOnlineLeaderboardYvr() = default;

	// IOnlineLeaderboard
	virtual bool ReadLeaderboards(const TArray< TSharedRef<const FUniqueNetId> >& Players, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsForFriends(int32 LocalUserNum, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsAroundRank(int32 Rank, uint32 Range, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsAroundUser(TSharedRef<const FUniqueNetId> Player, uint32 Range, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual void FreeStats(FOnlineLeaderboardRead& ReadObject) override;
	virtual bool WriteLeaderboards(const FName& SessionName, const FUniqueNetId& Player, FOnlineLeaderboardWrite& WriteObject) override;
	virtual bool FlushLeaderboards(const FName& SessionName) override;
	virtual bool WriteOnlinePlayerRatings(const FName& SessionName, int32 LeaderboardId, const TArray<FOnlinePlayerScore>& PlayerScores) override;
};