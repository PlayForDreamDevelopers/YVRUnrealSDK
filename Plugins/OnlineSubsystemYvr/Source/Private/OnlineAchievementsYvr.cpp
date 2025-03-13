#include "OnlineAchievementsYvr.h"
#include "OnlineIdentityYvr.h"
#include "OnlineMessageMultiTaskYvr.h"
#include "Templates/SharedPointer.h"

class FOnlineMessageMultiTaskYvrWriteAchievements : public FOnlineMessageMultiTaskYvr, public TSharedFromThis<FOnlineMessageMultiTaskYvrWriteAchievements>
{
private:

	FUniqueNetIdYvr PlayerId;
	FOnlineAchievementsWriteRef WriteObject;
	FOnAchievementsWrittenDelegate AchievementDelegate;

	// private to force the use of FOnlineMessageMultiTaskYvrWriteAchievements::Create()
	FOnlineMessageMultiTaskYvrWriteAchievements(FOnlineSubsystemYvr& InYvrSubsystem, const FUniqueNetIdYvr& InPlayerId, FOnlineAchievementsWriteRef& InWriteObject, const FOnAchievementsWrittenDelegate& InAchievementDelegate)
		: FOnlineMessageMultiTaskYvr(InYvrSubsystem, FOnlineMessageMultiTaskYvr::FFinalizeDelegate::CreateRaw(this, &FOnlineMessageMultiTaskYvrWriteAchievements::Finalize))
		, PlayerId(InPlayerId)
		, WriteObject(InWriteObject)
		, AchievementDelegate(InAchievementDelegate)
	{}

	static TSet< TSharedRef<FOnlineMessageMultiTaskYvrWriteAchievements> > ActiveAchievementWriteTasks;

PACKAGE_SCOPE:

	static TSharedRef<FOnlineMessageMultiTaskYvrWriteAchievements> Create(
		FOnlineSubsystemYvr& InYvrSubsystem,
		const FUniqueNetIdYvr& InPlayerId,
		FOnlineAchievementsWriteRef& InWriteObject,
		const FOnAchievementsWrittenDelegate& InAchievementDelegate)
	{
		TSharedRef<FOnlineMessageMultiTaskYvrWriteAchievements> NewTask = MakeShareable(new FOnlineMessageMultiTaskYvrWriteAchievements(InYvrSubsystem, InPlayerId, InWriteObject, InAchievementDelegate));
		ActiveAchievementWriteTasks.Add(NewTask);

		return NewTask;
	}

	void Finalize()
	{
		WriteObject->WriteState = (bDidAllRequestsFinishedSuccessfully) ? EOnlineAsyncTaskState::Done : EOnlineAsyncTaskState::Failed;
		AchievementDelegate.ExecuteIfBound(PlayerId, true);
		ActiveAchievementWriteTasks.Remove(AsShared());
	}

	static void ClearAllActiveTasks()
	{
		ActiveAchievementWriteTasks.Empty();
	}
};
TSet< TSharedRef<FOnlineMessageMultiTaskYvrWriteAchievements> > FOnlineMessageMultiTaskYvrWriteAchievements::ActiveAchievementWriteTasks;

FOnlineAchievementsYvr::FOnlineAchievementsYvr(class FOnlineSubsystemYvr& InSubsystem)
: YvrSubsystem(InSubsystem)
{
}

FOnlineAchievementsYvr::~FOnlineAchievementsYvr()
{
	FOnlineMessageMultiTaskYvrWriteAchievements::ClearAllActiveTasks();
}

void FOnlineAchievementsYvr::WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate)
{
#if PLATFORM_ANDROID
	if (AchievementDescriptions.Num() == 0)
	{
		WriteObject->WriteState = EOnlineAsyncTaskState::Failed;
		Delegate.ExecuteIfBound(PlayerId, false);
		return;
	}

	auto LoggedInPlayerId = YvrSubsystem.GetIdentityInterface()->GetUniquePlayerId(0);
	if (!(LoggedInPlayerId.IsValid() && PlayerId == *LoggedInPlayerId))
	{
		UE_LOG_ONLINE_ACHIEVEMENTS(Error, TEXT("Can only write achievements for logged in player id"));
		WriteObject->WriteState = EOnlineAsyncTaskState::Failed;
		Delegate.ExecuteIfBound(PlayerId, false);
		return;
	}

	if (WriteObject->Properties.Num() == 0)
	{
		WriteObject->WriteState = EOnlineAsyncTaskState::Done;
		Delegate.ExecuteIfBound(PlayerId, true);
		return;
	}

	WriteObject->WriteState = EOnlineAsyncTaskState::InProgress;
	TSharedRef<FOnlineMessageMultiTaskYvrWriteAchievements> MultiTask = FOnlineMessageMultiTaskYvrWriteAchievements::Create(YvrSubsystem, static_cast<FUniqueNetIdYvr>(PlayerId), WriteObject, Delegate);

	for (FStatPropertyArray::TConstIterator It(WriteObject->Properties); It; ++It)
	{
		const FString AchievementId = It.Key().ToString();
		auto VariantData = It.Value();

		auto AchievementDesc = AchievementDescriptions.Find(AchievementId);
		if (AchievementDesc == nullptr)
		{
			WriteObject->WriteState = EOnlineAsyncTaskState::Failed;
			Delegate.ExecuteIfBound(PlayerId, false);
			return;
		}

		UE_LOG_ONLINE_ACHIEVEMENTS(Verbose, TEXT("WriteObject AchievementId: '%s'"), *AchievementId);

		int RequestId = 0;

		switch (AchievementDesc->Type)
		{
			case EAchievementType::Simple:
			{
				RequestId = OnlineSubsystemYvrWrapper::UnlockAchievement(TCHAR_TO_ANSI(*AchievementId));
				break;
			}
			case EAchievementType::Count:
			{
				uint64 Count;
				GetWriteAchievementCountValue(VariantData, Count);
				RequestId = OnlineSubsystemYvrWrapper::AddCount(TCHAR_TO_ANSI(*AchievementId), Count);
				break;
			}
			case EAchievementType::Bitfield:
			{
				FString Bitfield;
				GetWriteAchievementBitfieldValue(VariantData, Bitfield, AchievementDesc->BitfieldLength);
				RequestId = OnlineSubsystemYvrWrapper::AddFields(TCHAR_TO_ANSI(*AchievementId), TCHAR_TO_ANSI(*Bitfield));
				break;
			}
			default:
			{
				UE_LOG_ONLINE_ACHIEVEMENTS(Warning, TEXT("Unknown achievement type"));
				break;
			}
		}

		if (RequestId != 0)
		{
			MultiTask->AddNewRequest(RequestId);
		}
	}
#endif
};

void FOnlineAchievementsYvr::QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate)
{
#if PLATFORM_ANDROID
	auto LoggedInPlayerId = YvrSubsystem.GetIdentityInterface()->GetUniquePlayerId(0);
	if (!(LoggedInPlayerId.IsValid() && PlayerId == *LoggedInPlayerId))
	{
		UE_LOG_ONLINE_ACHIEVEMENTS(Error, TEXT("Can only query for logged in player id"));
		Delegate.ExecuteIfBound(PlayerId, false);
		return;
	}

	auto YvrPlayerId = static_cast<FUniqueNetIdYvr>(PlayerId);
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetAllProgress(),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, YvrPlayerId, Delegate](YvrMessageHandle Message, bool bIsError)
	{
		if (bIsError)
		{
			Delegate.ExecuteIfBound(YvrPlayerId, false);
			return;
		}

		const size_t AchievementProgressNum = OnlineSubsystemYvrWrapper::GetSizeOfAchievementProgress(Message);

		TArray<FOnlineAchievement> AchievementsForPlayer;
		TSet<FString> InProgressAchievements;

		for (size_t AchievementProgressIndex = 0; AchievementProgressIndex < AchievementProgressNum; ++AchievementProgressIndex)
		{
			auto AchievementProgress = OnlineSubsystemYvrWrapper::GetElementOfAchievementProgress(Message, AchievementProgressIndex);
			FOnlineAchievementYvr NewAchievement(AchievementProgress);
			NewAchievement.Progress = CalculatePlayerAchievementProgress(NewAchievement);

			AchievementsForPlayer.Add(NewAchievement);
			InProgressAchievements.Add(NewAchievement.Id);
		}

		for (auto const &it : AchievementDescriptions)
		{
			auto bFoundAchievement = InProgressAchievements.Find(it.Key);
			if (bFoundAchievement == nullptr)
			{
				FOnlineAchievementYvr NewAchievement(it.Value);
				AchievementsForPlayer.Add(NewAchievement);
			}
		}

		PlayerAchievements.Add(YvrPlayerId, AchievementsForPlayer);

		Delegate.ExecuteIfBound(YvrPlayerId, true);
	}));
#endif
}

void FOnlineAchievementsYvr::QueryAchievementDescriptions(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate)
{
#if PLATFORM_ANDROID
	auto YvrPlayerId = static_cast<FUniqueNetIdYvr>(PlayerId);
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetAllDefinitions(),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, YvrPlayerId, Delegate](YvrMessageHandle Message, bool bIsError)
	{
		if (bIsError)
		{
			Delegate.ExecuteIfBound(YvrPlayerId, false);
			return;
		}

		const size_t AchievementDefinitionNum = OnlineSubsystemYvrWrapper::GetSizeOfAllDefinitions(Message);
		for (size_t AchievementDefinitionIndex = 0; AchievementDefinitionIndex < AchievementDefinitionNum; ++AchievementDefinitionIndex)
		{
			auto AchievementDefinition = OnlineSubsystemYvrWrapper::GetElementOfDefinitions(Message, AchievementDefinitionIndex);
			FOnlineAchievementDescYvr NewAchievementDesc;
			FString Title(OnlineSubsystemYvrWrapper::GetApiNameFromElementOfDefinitions(AchievementDefinition));
			NewAchievementDesc.Title = FText::FromString(Title);
			NewAchievementDesc.bIsHidden = false;
			auto AchievementType = OnlineSubsystemYvrWrapper::GetAchievementTypeFromElementOfDefinitions(AchievementDefinition);
			NewAchievementDesc.Type = static_cast<EAchievementType>(AchievementType);
			NewAchievementDesc.Target = OnlineSubsystemYvrWrapper::GetTargetFromElementOfDefinitions(AchievementDefinition);
			NewAchievementDesc.BitfieldLength = OnlineSubsystemYvrWrapper::GetBitfieldLengthFromElementOfDefinitions(AchievementDefinition);

			AchievementDescriptions.Add(Title, NewAchievementDesc);
		}

		Delegate.ExecuteIfBound(YvrPlayerId, true);
	}));

#endif
}

EOnlineCachedResult::Type FOnlineAchievementsYvr::GetCachedAchievement(const FUniqueNetId& PlayerId, const FString& AchievementId, FOnlineAchievement& OutAchievement)
{
	if (AchievementDescriptions.Num() == 0)
	{
		// we don't have achievements
		return EOnlineCachedResult::NotFound;
	}

	auto YvrPlayerId = static_cast<const FUniqueNetIdYvr&>(PlayerId);
	const TArray<FOnlineAchievement> * PlayerAch = PlayerAchievements.Find(YvrPlayerId);
	if (PlayerAch == nullptr)
	{
		// achievements haven't been read for a player
		return EOnlineCachedResult::NotFound;
	}

	const int32 AchNum = PlayerAch->Num();
	for (int32 AchIdx = 0; AchIdx < AchNum; ++AchIdx)
	{
		if ((*PlayerAch)[AchIdx].Id == AchievementId)
		{
			OutAchievement = (*PlayerAch)[AchIdx];
			return EOnlineCachedResult::Success;
		}
	}

	// no such achievement
	return EOnlineCachedResult::NotFound;
};

EOnlineCachedResult::Type FOnlineAchievementsYvr::GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement> & OutAchievements)
{
	if (AchievementDescriptions.Num() == 0)
	{
		// we don't have achievements
		return EOnlineCachedResult::NotFound;
	}

	auto YvrPlayerId = static_cast<const FUniqueNetIdYvr&>(PlayerId);
	const TArray<FOnlineAchievement> * PlayerAch = PlayerAchievements.Find(YvrPlayerId);
	if (PlayerAch == nullptr)
	{
		// achievements haven't been read for a player
		return EOnlineCachedResult::NotFound;
	}

	OutAchievements = *PlayerAch;
	return EOnlineCachedResult::Success;
};

EOnlineCachedResult::Type FOnlineAchievementsYvr::GetCachedAchievementDescription(const FString& AchievementId, FOnlineAchievementDesc& OutAchievementDesc)
{
	if (AchievementDescriptions.Num() == 0)
	{
		// we don't have achievements
		return EOnlineCachedResult::NotFound;
	}

	FOnlineAchievementDesc * AchDesc = AchievementDescriptions.Find(AchievementId);
	if (AchDesc == nullptr)
	{
		// no such achievement
		return EOnlineCachedResult::NotFound;
	}

	OutAchievementDesc = *AchDesc;
	return EOnlineCachedResult::Success;
};

#if !UE_BUILD_SHIPPING
bool FOnlineAchievementsYvr::ResetAchievements(const FUniqueNetId& PlayerId)
{
	// We cannot reset achievements from the client
	UE_LOG_ONLINE_ACHIEVEMENTS(Error, TEXT("Achievements cannot be reset here"));
	return false;
};
#endif // !UE_BUILD_SHIPPING

void FOnlineAchievementsYvr::GetWriteAchievementCountValue(FVariantData VariantData, uint64& OutData) const
{
	switch (VariantData.GetType())
	{
		case EOnlineKeyValuePairDataType::Int32:
		{
			int32 Value;
			VariantData.GetValue(Value);
			OutData = static_cast<uint64>(Value);
			break;
		}
		case EOnlineKeyValuePairDataType::Int64:
		{
			int64 Value;
			VariantData.GetValue(Value);
			OutData = static_cast<uint64>(Value);
			break;
		}
		case EOnlineKeyValuePairDataType::UInt32:
		{
			uint32 Value;
			VariantData.GetValue(Value);
			OutData = static_cast<uint64>(Value);
			break;
		}
		case EOnlineKeyValuePairDataType::UInt64:
		{
			VariantData.GetValue(OutData);
			break;
		}
		default:
		{
			UE_LOG_ONLINE_ACHIEVEMENTS(Warning, TEXT("Could not %s convert to uint64"), VariantData.GetTypeString());
			OutData = 0;
			break;
		}
	}
}
void FOnlineAchievementsYvr::GetWriteAchievementBitfieldValue(FVariantData VariantData, FString& OutData, uint32 BitfieldLength) const
{
	switch (VariantData.GetType())
	{
		case EOnlineKeyValuePairDataType::Int32:
		{
			int32 Value;
			VariantData.GetValue(Value);
			auto UnpaddedBitfield = FString::FromInt(Value);
			auto PaddingLength = BitfieldLength - UnpaddedBitfield.Len();
			OutData = TEXT("");
			for (uint32 i = 0; i < PaddingLength; ++i)
			{
				OutData.AppendChar('0');
			}
			OutData.Append(UnpaddedBitfield);
			break;
		}
		case EOnlineKeyValuePairDataType::String:
		{
			VariantData.GetValue(OutData);
			break;
		}
		default:
		{
			UE_LOG_ONLINE_ACHIEVEMENTS(Warning, TEXT("Could not %s convert to string"), VariantData.GetTypeString());
			break;
		}
	}
}

double FOnlineAchievementsYvr::CalculatePlayerAchievementProgress(const FOnlineAchievementYvr Achievement)
{
	if (Achievement.bIsUnlocked)
	{
		return 100.0;
	}

	auto Desc = AchievementDescriptions.Find(Achievement.Id);
	if (Desc == nullptr)
	{
		UE_LOG_ONLINE_ACHIEVEMENTS(Warning, TEXT("Could not calculate progress for Achievement: '%s'"), *Achievement.Id);
		return 0.0;
	}

	double Progress;
	switch (Desc->Type)
	{
		case EAchievementType::Count:
		{
			Progress = Achievement.Count * 100.0 / Desc->Target;
			break;
		}
		case EAchievementType::Bitfield:
		{
			int BitfieldCount = 0;
			for (int32 i = 0; i < Achievement.Bitfield.Len(); ++i)
			{
				if (Achievement.Bitfield[i] == '1')
				{
					++BitfieldCount;
				}
			}
			Progress = BitfieldCount * 100.0 / Desc->Target;
			break;
		}
		default:
		{
			Progress = 0.0;
			break;
		}
	}

	// Cap the progress to 100
	return (Progress <= 100.0) ? Progress : 100.0;
}
