// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineMessageTaskManagerYvr.h"
#include "OnlineSubsystemYvrModule.h"

typedef TUniquePtr<class FOnlineMessageTaskManagerYvr> FOnlineMessageTaskManagerYvrPtr;
typedef TSharedPtr<class FOnlineIdentityYvr, ESPMode::ThreadSafe> FOnlineIdentityYvrPtr;
typedef TSharedPtr<class FOnlineAchievementsYvr, ESPMode::ThreadSafe> FOnlineAchievementsYvrPtr;
typedef TSharedPtr<class FOnlineFriendsYvr, ESPMode::ThreadSafe> FOnlineFriendsYvrPtr;
typedef TSharedPtr<class FOnlineLeaderboardYvr, ESPMode::ThreadSafe> FOnlineLeaderboardYvrPtr;

//Yvr Platform
typedef TSharedPtr<class FYvrPlatformIAPInterface, ESPMode::ThreadSafe> FYvrPlatformIAPInterfacePtr;

//Yvr Sport
typedef TSharedPtr<class FOnlineSportYvr, ESPMode::ThreadSafe> FOnlineSportYvrPtr;

class ONLINESUBSYSTEMYVR_API FOnlineSubsystemYvr : public FOnlineSubsystemImpl
{
public:
	virtual ~FOnlineSubsystemYvr() = default;

	// IOnlineSubsystem
	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineGroupsPtr GetGroupsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override;
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual IOnlineChatPtr GetChatInterface() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;

	//Yvr Platform
	FYvrPlatformIAPInterfacePtr GetYvrPlatformIAPInterface() const;

	//Yvr Sport
	FOnlineSportYvrPtr GetYvrOnlineSportYvrPtr() const;

	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

	// FTickerObjectBase

	virtual bool Tick(float DeltaTime) override;

	/**
	* Allows for the LibOVRPlatform calls to be used directly with the Delegates in the Yvr OSS
	*/
	void AddRequestDelegate(uint64 RequestId, FYvrMessageOnCompleteDelegate&& Delegate) const;

	/**
	* Allows for direct subscription to the LibOVRPlatform notifications with the Delegates in the Yvr OSS
	*/
	FYvrMulticastMessageOnCompleteDelegate& GetNotifyDelegate(YvrMessageType MessageType) const;
	void RemoveNotifyDelegate(YvrMessageType MessageType, const FDelegateHandle& Delegate) const;

    /** Only the factory makes instances */
    FOnlineSubsystemYvr() = delete;
	explicit FOnlineSubsystemYvr(FName InInstanceName)
	:FOnlineSubsystemImpl(YVR_SUBSYSTEM, InInstanceName)
	,bYvrInit(false)
	{
		
	}

	bool IsInitialized() const{return bYvrInit;};

	bool InitWithAndroidPlatform();

private:

	bool bYvrInit;
	/** Message Task Manager */
	FOnlineMessageTaskManagerYvrPtr MessageTaskManager;

	/** Interface to the identity registration/auth services */
	FOnlineIdentityYvrPtr IdentityInterface;

	/** Interface for achievements */
	FOnlineAchievementsYvrPtr AchievementsInterface;

	/** Interface for friends */
	FOnlineFriendsYvrPtr FriendsInterface;

	/** Interface for Leader board */
	FOnlineLeaderboardYvrPtr LeaderboardInterface;

	//Yvr Platform
	FYvrPlatformIAPInterfacePtr YvrIAPInterface;

	//Yvr Sport
	FOnlineSportYvrPtr OnlineSportYvr;
};

namespace FNetworkProtocolTypes
{
	ONLINESUBSYSTEMYVR_API extern const FName Yvr;
}

typedef TSharedPtr<FOnlineSubsystemYvr, ESPMode::ThreadSafe> FOnlineSubsystemYvrPtr;
