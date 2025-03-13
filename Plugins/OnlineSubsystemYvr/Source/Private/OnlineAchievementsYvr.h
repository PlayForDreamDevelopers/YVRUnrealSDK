// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OnlineIdentityYvr.h"
#include "OnlineSubsystemYvrWrapper.h"

enum class EAchievementType {
	Unknown = -1,
	Simple,
	Count,
	Bitfield
};

struct FOnlineAchievementDescYvr : FOnlineAchievementDesc
{
	EAchievementType Type;
	uint64 Target;
	uint32 BitfieldLength;
};

struct FOnlineAchievementYvr : FOnlineAchievement
{
	FOnlineAchievementYvr(const YvrMessageHandle& AchievementProgress)
	{
#if PLATFORM_ANDROID
		Count = OnlineSubsystemYvrWrapper::GetCountProgressFromElementOfAchievementProgress(AchievementProgress);
		Bitfield = OnlineSubsystemYvrWrapper::GetBitfieldProgressFromElementOfAchievementProgress(AchievementProgress);
		bIsUnlocked = OnlineSubsystemYvrWrapper::IsUnlockedFromElementOfAchievementProgress(AchievementProgress);
		Id = OnlineSubsystemYvrWrapper::GetNameFromDefinitionOfAchievementProgress(AchievementProgress);
#else
		Count = 0;
		Bitfield = "";
		bIsUnlocked = false;
		Id = "";
#endif
	}

	FOnlineAchievementYvr(const FOnlineAchievementDescYvr& AchievementDesc) :
		Count(0),
		bIsUnlocked(false)
	{
		Id = AchievementDesc.Title.ToString();
		Progress = 0;
		if (AchievementDesc.Type == EAchievementType::Bitfield)
		{
			Bitfield = TEXT("");
			for (uint32 i = 0; i < AchievementDesc.BitfieldLength; ++i)
			{
				Bitfield.AppendChar('0');
			}
		}
	}

	uint64 Count;
	FString Bitfield;
	bool bIsUnlocked;
};


class FOnlineAchievementsYvr : public IOnlineAchievements
{
private:
	FOnlineSubsystemYvr& YvrSubsystem;

	TMap<FUniqueNetIdYvr, TArray<FOnlineAchievement>> PlayerAchievements;

	TMap<FString, FOnlineAchievementDescYvr> AchievementDescriptions;

	void GetWriteAchievementCountValue(FVariantData VariantData, uint64& OutData) const;
	void GetWriteAchievementBitfieldValue(FVariantData VariantData, FString& OutData, uint32 BitfieldLength) const;
	double CalculatePlayerAchievementProgress(const FOnlineAchievementYvr Achievement);

public:

	FOnlineAchievementsYvr(FOnlineSubsystemYvr& InSubsystem);
	virtual ~FOnlineAchievementsYvr();

	// Begin IOnlineAchievements interface
	virtual void WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate = FOnAchievementsWrittenDelegate()) override;
	virtual void QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate()) override;
	virtual void QueryAchievementDescriptions(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate()) override;
	virtual EOnlineCachedResult::Type GetCachedAchievement(const FUniqueNetId& PlayerId, const FString& AchievementId, FOnlineAchievement& OutAchievement) override;
	virtual EOnlineCachedResult::Type GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement> & OutAchievements) override;
	virtual EOnlineCachedResult::Type GetCachedAchievementDescription(const FString& AchievementId, FOnlineAchievementDesc& OutAchievementDesc) override;
#if !UE_BUILD_SHIPPING
	virtual bool ResetAchievements(const FUniqueNetId& PlayerId) override;
#endif // !UE_BUILD_SHIPPING
};
