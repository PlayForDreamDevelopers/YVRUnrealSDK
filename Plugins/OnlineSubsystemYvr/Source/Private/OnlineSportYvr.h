#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemYvr.h"

DECLARE_LOG_CATEGORY_EXTERN(YvrSportIAP, Log, All);

struct FUserInfo
{
	int Gender;
	int BirthDay;
	int Stature;
	int Weight;
	int PlanDurationInMinutes;
	int PlanColorie;
	int DaysPerWeek;
};

struct FSummary
{
	long DurationInSeconds;
	double Calorie;
};

struct FDailySummary
{
	FString DateTime;
	int DurationInSeconds;
	int PlanDurationInMinutes;
	double Calorie;
	double PlanCalorie;
};

DECLARE_DELEGATE_OneParam(FGetUserInfoDelegate, const FUserInfo);
DECLARE_DELEGATE_OneParam(FGetSummaryDelegate, const FSummary);
DECLARE_DELEGATE_OneParam(FGetDailySummaryDelegate, const TArray<FDailySummary>);

class ONLINESUBSYSTEMYVR_API FOnlineSportYvr
{
private:
	FOnlineSubsystemYvr& YvrSubsystem;

public:
	FOnlineSportYvr(FOnlineSubsystemYvr& InSubsystem);
	~FOnlineSportYvr();
	void GetUserInfo(FGetUserInfoDelegate Delegate);
	void GetSummary(long beginTime, long endTime, FGetSummaryDelegate Delegate);
	void GetSummary(FString beginTime, FString endTime, FGetSummaryDelegate Delegate);
	void GetDailySummary(long beginTime, long endTime, FGetDailySummaryDelegate Delegate);
	void GetDailySummary(FString beginTime, FString endTime, FGetDailySummaryDelegate Delegate);
};
