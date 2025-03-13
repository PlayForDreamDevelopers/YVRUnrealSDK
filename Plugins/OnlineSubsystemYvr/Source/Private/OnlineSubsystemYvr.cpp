#include "OnlineSubsystemYvr.h"
#include "Misc/ConfigCacheIni.h"
#include "OnlineSubsystemYvrWrapper.h"
#include "OnlineIdentityYvr.h"
#include "OnlineAchievementsYvr.h"
#include "OnlineFriendsYvr.h"
#include "OnlineSettingsYvr.h"
#include "OnlineLeaderboardYvr.h"
#include "YvrPlatformIAPInterface.h"
#include "OnlineSportYvr.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#endif


namespace FNetworkProtocolTypes
{
	const FName Yvr(TEXT("Yvr"));
}


IOnlineSessionPtr FOnlineSubsystemYvr::GetSessionInterface() const
{
	return nullptr;
}

IOnlineGroupsPtr FOnlineSubsystemYvr::GetGroupsInterface() const
{
	return nullptr;
}

IOnlineFriendsPtr FOnlineSubsystemYvr::GetFriendsInterface() const
{
	return FriendsInterface;
}

IOnlineSharedCloudPtr FOnlineSubsystemYvr::GetSharedCloudInterface() const
{
	return nullptr;
}

IOnlineUserCloudPtr FOnlineSubsystemYvr::GetUserCloudInterface() const
{
	return nullptr;
}

IOnlineEntitlementsPtr FOnlineSubsystemYvr::GetEntitlementsInterface() const
{
	return nullptr;
}

IOnlineLeaderboardsPtr FOnlineSubsystemYvr::GetLeaderboardsInterface() const
{
	return LeaderboardInterface;
}
;

IOnlineVoicePtr FOnlineSubsystemYvr::GetVoiceInterface() const
{
	return nullptr;
}

IOnlineExternalUIPtr FOnlineSubsystemYvr::GetExternalUIInterface() const
{
	return nullptr;
}

IOnlineTimePtr FOnlineSubsystemYvr::GetTimeInterface() const
{
	return nullptr;
}

IOnlineIdentityPtr FOnlineSubsystemYvr::GetIdentityInterface() const
{
	return IdentityInterface;
}

IOnlinePartyPtr FOnlineSubsystemYvr::GetPartyInterface() const
{
	return nullptr;
}

IOnlineTitleFilePtr FOnlineSubsystemYvr::GetTitleFileInterface() const
{
	return nullptr;
}

IOnlineStoreV2Ptr FOnlineSubsystemYvr::GetStoreV2Interface() const
{
	return nullptr;
}

IOnlinePurchasePtr FOnlineSubsystemYvr::GetPurchaseInterface() const
{
	return nullptr;
}

IOnlineEventsPtr FOnlineSubsystemYvr::GetEventsInterface() const
{
	return nullptr;
}

IOnlineAchievementsPtr FOnlineSubsystemYvr::GetAchievementsInterface() const
{
	return AchievementsInterface;
}

IOnlineSharingPtr FOnlineSubsystemYvr::GetSharingInterface() const
{
	return nullptr;
}

IOnlineUserPtr FOnlineSubsystemYvr::GetUserInterface() const
{
	return nullptr;
}

IOnlineMessagePtr FOnlineSubsystemYvr::GetMessageInterface() const
{
	return nullptr;
}

IOnlinePresencePtr FOnlineSubsystemYvr::GetPresenceInterface() const
{
	return nullptr;
}

IOnlineChatPtr FOnlineSubsystemYvr::GetChatInterface() const
{
	return nullptr;
}

IOnlineStatsPtr FOnlineSubsystemYvr::GetStatsInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemYvr::GetTurnBasedInterface() const
{
	return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemYvr::GetTournamentInterface() const
{
	return nullptr;
}

FYvrPlatformIAPInterfacePtr FOnlineSubsystemYvr::GetYvrPlatformIAPInterface() const
{
	return YvrIAPInterface;
}

FOnlineSportYvrPtr FOnlineSubsystemYvr::GetYvrOnlineSportYvrPtr() const
{
	return OnlineSportYvr;
}

bool FOnlineSubsystemYvr::Init()
{
	bYvrInit = false;
#if PLATFORM_ANDROID
	bYvrInit = InitWithAndroidPlatform();
#endif
	if (bYvrInit)
	{
		MessageTaskManager = MakeUnique<FOnlineMessageTaskManagerYvr>();
		check(MessageTaskManager);

		IdentityInterface = MakeShareable(new FOnlineIdentityYvr(*this));
		AchievementsInterface = MakeShareable(new FOnlineAchievementsYvr(*this));
		FriendsInterface = MakeShareable(new FOnlineFriendsYvr(*this));
		LeaderboardInterface = MakeShareable(new FOnlineLeaderboardYvr(*this));

		//Yvr Platform
		YvrIAPInterface = MakeShareable(new FYvrPlatformIAPInterface(*this));

		//Yvr Sport FOnlineSportYvr
		OnlineSportYvr = MakeShareable(new FOnlineSportYvr(*this));
		StartTicker();
	}
	else
	{
		FOnlineSubsystemImpl::Shutdown();
	}
	return bYvrInit;
}

bool FOnlineSubsystemYvr::Tick(float DeltaTime)
{
	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}
	if (MessageTaskManager.IsValid())
	{
		if (!MessageTaskManager->Tick(DeltaTime))
		{
			UE_LOG_ONLINE(Error, TEXT("An error occured when processing the message queue"));
		}
	}
	return true;
}

void FOnlineSubsystemYvr::AddRequestDelegate(uint64 RequestId, FYvrMessageOnCompleteDelegate&& Delegate) const
{
	check(MessageTaskManager);
	UE_LOG(LogTemp, Log, TEXT("AddRequestDelegate %llu"), RequestId);
	MessageTaskManager->AddRequestDelegate(RequestId, MoveTemp(Delegate));
}

FYvrMulticastMessageOnCompleteDelegate& FOnlineSubsystemYvr::GetNotifyDelegate(YvrMessageType MessageType) const
{
	check(MessageTaskManager);
	return MessageTaskManager->GetNotifyDelegate(MessageType);
}

void FOnlineSubsystemYvr::RemoveNotifyDelegate(YvrMessageType MessageType, const FDelegateHandle& Delegate) const
{
	check(MessageTaskManager);
	return MessageTaskManager->RemoveNotifyDelegate(MessageType, Delegate);
}

bool FOnlineSubsystemYvr::InitWithAndroidPlatform()
{
#if PLATFORM_ANDROID
	long AppId = FCString::Atoi64(*GetAppId());
	auto result = OnlineSubsystemYvrWrapper::InitPlatformSDK(AppId);
	if (result)
	{
		UE_LOG_ONLINE(Log, TEXT("platform init success %ld"), AppId);
		return true;
	}
#endif

	UE_LOG_ONLINE(Log, TEXT("platform init fail"));
	return false;
}

bool FOnlineSubsystemYvr::Shutdown()
{
	FOnlineSubsystemImpl::Shutdown();
	return true;
}

FString FOnlineSubsystemYvr::GetAppId() const
{
	return GConfig->GetStr(TEXT("/Script/OnlineSubsystemYvr.OnlineSettingsYvr"), TEXT("AppId"), GEngineIni);
}

bool FOnlineSubsystemYvr::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

FText FOnlineSubsystemYvr::GetOnlineServiceName() const
{
	return NSLOCTEXT("OnlineSubsystemYvr", "OnlineServiceName", "Yvr Platform");
}