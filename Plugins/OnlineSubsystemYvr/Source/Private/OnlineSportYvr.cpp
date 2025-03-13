#include "OnlineSportYvr.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemYvrWrapper.h"

DEFINE_LOG_CATEGORY(YvrSportIAP);

FOnlineSportYvr::FOnlineSportYvr(FOnlineSubsystemYvr& InSubsystem):
	YvrSubsystem(InSubsystem)
{
}

FOnlineSportYvr::~FOnlineSportYvr()
{
}

void FOnlineSportYvr::GetUserInfo(FGetUserInfoDelegate Delegate)
{
#if PLATFORM_ANDROID
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetUseInfo(),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
		{
			FUserInfo NewUserInfo{};
			if (bIsError)
			{
				auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
		   		UE_LOG(YvrSportIAP, Log, TEXT("GetUserInfo failed: %s"), *ErrorMessage);
			    Delegate.ExecuteIfBound(NewUserInfo);
		    }
		    else
		    {
		   		NewUserInfo.Gender = OnlineSubsystemYvrWrapper::GetGender(Message);
		   		NewUserInfo.BirthDay = OnlineSubsystemYvrWrapper::GetBirthDay(Message);
		   		NewUserInfo.Stature = OnlineSubsystemYvrWrapper::GetStature(Message);
		   		NewUserInfo.Weight = OnlineSubsystemYvrWrapper::GetWeight(Message);
		   		NewUserInfo.PlanDurationInMinutes = OnlineSubsystemYvrWrapper::GetPlanDurationInSeconds(Message);
		   		NewUserInfo.PlanColorie = OnlineSubsystemYvrWrapper::GetPlanCalorie(Message);
		   		NewUserInfo.DaysPerWeek = OnlineSubsystemYvrWrapper::GetDaysPerWeek(Message);
		    	Delegate.ExecuteIfBound(NewUserInfo);
		    }
	    }));
#endif
}

void FOnlineSportYvr::GetSummary(long beginTime, long endTime, FGetSummaryDelegate Delegate)
{
#if PLATFORM_ANDROID
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetSummary(beginTime, endTime),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
		{
			FSummary NewSummary{};
			if (bIsError)
			{
				auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
				UE_LOG(YvrSportIAP, Log, TEXT("GetSummary failed: %s"), *ErrorMessage);
				Delegate.ExecuteIfBound(NewSummary);
			}
			else
			{
		   		NewSummary.DurationInSeconds = OnlineSubsystemYvrWrapper::GetSummaryDurationInSeconds(Message);
		   		NewSummary.Calorie = OnlineSubsystemYvrWrapper::GetSummaryCalorie(Message);
				Delegate.ExecuteIfBound(NewSummary);
			}
		}));
#endif
}

void FOnlineSportYvr::GetSummary(FString beginTime, FString endTime, FGetSummaryDelegate Delegate)
{
#if PLATFORM_ANDROID
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetSummary(beginTime, endTime),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
		{
			FSummary NewSummary{};
			if (bIsError)
			{
				auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
				UE_LOG(YvrSportIAP, Log, TEXT("GetSummary failed: %s"), *ErrorMessage);
				Delegate.ExecuteIfBound(NewSummary);
			}
			else
			{
				NewSummary.DurationInSeconds = OnlineSubsystemYvrWrapper::GetSummaryDurationInSeconds(Message);
				NewSummary.Calorie = OnlineSubsystemYvrWrapper::GetSummaryCalorie(Message);
				Delegate.ExecuteIfBound(NewSummary);
			}
		}));
#endif
}

void FOnlineSportYvr::GetDailySummary(long beginTime, long endTime, FGetDailySummaryDelegate Delegate)
{
#if PLATFORM_ANDROID
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetDailySummary(beginTime, endTime),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
		{
			TArray<FDailySummary> DailySummaryList;
			if (bIsError)
			{
				auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
				UE_LOG(YvrSportIAP, Log, TEXT("GetDailySummary failed:%s"), *ErrorMessage);
				Delegate.ExecuteIfBound(DailySummaryList);
			}
			else
			{
				int DailySummarySize = OnlineSubsystemYvrWrapper::GetDailySummarySize(Message);
				for (int i = 0; i < DailySummarySize; ++i)
				{
					auto DailySummary = OnlineSubsystemYvrWrapper::GetDailySummaryByIndex(Message, i);

					FDailySummary NewDailySummary{};
					NewDailySummary.DateTime = OnlineSubsystemYvrWrapper::GetDailySummaryDate(DailySummary);
					NewDailySummary.DurationInSeconds = OnlineSubsystemYvrWrapper::GetDailySummaryDurationInSeconds(DailySummary);
					NewDailySummary.PlanDurationInMinutes = OnlineSubsystemYvrWrapper::GetDailySummaryPlanDurationInSeconds(DailySummary) / 60;
					NewDailySummary.PlanCalorie = OnlineSubsystemYvrWrapper::GetDailySummaryPlanCalorie(DailySummary);
					NewDailySummary.Calorie = OnlineSubsystemYvrWrapper::GetDailySummaryCalorie(DailySummary);
					DailySummaryList.Add(NewDailySummary);
				}
				Delegate.ExecuteIfBound(DailySummaryList);
			}
		}));
#endif
}

void FOnlineSportYvr::GetDailySummary(FString beginTime, FString endTime, FGetDailySummaryDelegate Delegate)
{
#if PLATFORM_ANDROID
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetDailySummary(beginTime, endTime),
		FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
		{
			TArray<FDailySummary> DailySummaryList;
			if (bIsError)
			{
				auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
				UE_LOG(YvrSportIAP, Log, TEXT("GetDailySummary failed:%s"), *ErrorMessage);
				Delegate.ExecuteIfBound(DailySummaryList);
			}
			else
			{
				int DailySummarySize = OnlineSubsystemYvrWrapper::GetDailySummarySize(Message);
				for (int i = 0; i < DailySummarySize; ++i)
				{
					auto DailySummary = OnlineSubsystemYvrWrapper::GetDailySummaryByIndex(Message, i);

					FDailySummary NewDailySummary{};
					NewDailySummary.DateTime = OnlineSubsystemYvrWrapper::GetDailySummaryDate(DailySummary);
					NewDailySummary.DurationInSeconds = OnlineSubsystemYvrWrapper::GetDailySummaryDurationInSeconds(DailySummary);
					NewDailySummary.PlanDurationInMinutes = OnlineSubsystemYvrWrapper::GetDailySummaryPlanDurationInSeconds(DailySummary) / 60;
					NewDailySummary.PlanCalorie = OnlineSubsystemYvrWrapper::GetDailySummaryPlanCalorie(DailySummary);
					NewDailySummary.Calorie = OnlineSubsystemYvrWrapper::GetDailySummaryCalorie(DailySummary);
					DailySummaryList.Add(NewDailySummary);
				}
				Delegate.ExecuteIfBound(DailySummaryList);
			}
		}));
#endif
}
