// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystemYvrTypes.h"


class OnlineSubsystemYvrWrapper
{
public:
#if PLATFORM_ANDROID
	static bool InitPlatformSDK(long AppId);

	//Request
	static jobject PopMessage();
	static int GetRequestId(jobject Obj);
	static FString GetRequestType(jobject Obj);
	static bool IsMessageError(jobject Obj);
	static FString GetErrorMessage(jobject Obj);
	static int GetErrorCode(jobject Obj);
	static void FreeRequest(int RequestId);

	//User
	static int GetLoggedInUser();
	static int GetAccountId(jobject Obj);
	static FString GetUserName(jobject Obj);
	static FString GetUserIcon(jobject Obj);
	static int GetUserSex(jobject Obj);

	//Achievement
	static int AddCount(FString Name, long Count);
	static int AddFields(FString Name, FString Fields);
	static int UnlockAchievement(FString Name);
	//Achievement Definition
	static int GetAllDefinitions();
	static int GetDefinitionByNames(TArray<FString> Names);
	static int GetSizeOfAllDefinitions(jobject Obj);
	static jobject GetElementOfDefinitions(jobject Obj, int Index);
	static int GetIdFromElementOfDefinitions(jobject Obj);
	static FString GetApiNameFromElementOfDefinitions(jobject Obj);
	static int GetAchievementTypeFromElementOfDefinitions(jobject Obj);
	static int GetPolicyFromElementOfDefinitions(jobject Obj);
	static int GetTargetFromElementOfDefinitions(jobject Obj);
	static int GetBitfieldLengthFromElementOfDefinitions(jobject Obj);
	static bool IsAchievedFromElementOfDefinitions(jobject Obj);
	static FString GetTitleFromElementOfDefinitions(jobject Obj);
	static FString GetDescriptionFromElementOfDefinitions(jobject Obj);
	static FString GetUnlockedDescriptionFromElementOfDefinitions(jobject Obj);
	static bool IsSecretFromElementOfDefinitions(jobject Obj);
	static FString GetLockedImageFromElementOfDefinitions(jobject Obj);
	static FString GetUnlockedImageFromElementOfDefinitions(jobject Obj);
	static long GetCreatedTimeFromElementOfDefinitions(jobject Obj);
	static long GetUpdateTimeFromElementOfDefinitions(jobject Obj);
	//Achievement Progress
	static int GetAllProgress();
	static int GetProgressByName(TArray<FString> Names);
	static int GetSizeOfAchievementProgress(jobject Obj);
	static jobject GetElementOfAchievementProgress(jobject Obj, int Index);
	static int GetIdFromElementOfAchievementProgress(jobject Obj);
	static jobject GetDefinitionFromElementOfAchievementProgress(jobject Obj);
	static FString GetNameFromDefinitionOfAchievementProgress(jobject Obj);
	static int GetTargetFromDefinitionOfAchievementProgress(jobject Obj);
	static int GetCountProgressFromElementOfAchievementProgress(jobject Obj);
	static FString GetBitfieldProgressFromElementOfAchievementProgress(jobject Obj);
	static bool IsUnlockedFromElementOfAchievementProgress(jobject Obj);
	static long GetUnlockTimeFromElementOfAchievementProgress(jobject Obj);

	//Entitlement
	static int GetViewerEntitled();
	static bool IsViewerEntitled(jobject Obj);

	//Friends
	static int GetYvrFriends();
	static int GetFriendsSize(jobject Obj);
	static jobject GetItemOfFriendsList(jobject Obj, int Index);
	static int GetActIdOfFriendItem(jobject Obj);
	static FString GetNickOfFriendItem(jobject Obj);
	static int GetAgeOfFriendItem(jobject Obj);
	static int GetSexOfFriendItem(jobject Obj);
	static FString GetIconOfFriendItem(jobject Obj);
	static int GetOnlineOfFriendItem(jobject Obj);
	static jobject GetUsingAppOfFriendItem(jobject Obj);
	static FString GetScoverOfUsingApp(jobject Obj);
	static int GetTypeOfUsingApp(jobject Obj);
	static FString GetPkgOfUsingApp(jobject Obj);
	static FString GetNameOfUsingApp(jobject Obj);

	//User
	static int GetYvrFriendInfo(int Index);
	static int GetActIdOfUser(jobject Obj);
	static FString GetNickOfUser(jobject Obj);
	static int GetAgeOfUser(jobject Obj);
	static int GetSexOfUser(jobject Obj);
	static FString GetIconOfUser(jobject Obj);
	static int GetOnlineOfUser(jobject Obj);

	//Leaderboard
	static int WriteLeaderboardItem(FString Name, float Score, void* Data, int DataLength, bool ForceUpdate);
	static int ReadLeaderboardItems(FString Name, long Size, FString FilterType, FString StartAt);
	static jobject GetLeaderboardList(jobject Obj);
	static int GetLeaderboardItemSize(jobject Obj);
	static jobject GetItemOfLeaderboard(jobject Obj, int Index);
	static long GetLeaderboardUserId(jobject Obj);
	static FString GetLeaderboardUserNickName(jobject Obj);
	static long GetLeaderboardRank(jobject Obj);
	static double GetLeaderboardScore(jobject Obj);

	//IAP
	static int GetViewerPurchases();
	static int GetPurchaseSize(jobject Obj);
	static jobject GetPurchaseByMessage(jobject Obj);
	static jobject GetPurchaseByIndex(jobject Obj, int Index);
	static FString GetSkuOfPurchase(jobject Obj);
	static FString GetNameOfPurchase(jobject Obj);
	static int GetTypeOfPurchase(jobject Obj);
	static FString GetScoverOfPurchase(jobject Obj);
	static FString GetTradeNoOfPurchase(jobject Obj);

	static int GetProductsBySKU(TArray<FString> Skus);
	static int GetProductSize(jobject Obj);
	static jobject GetProductByIndex(jobject Obj, int Index);
	static FString GetSkuOfProduct(jobject Obj);
	static FString GetNameOfProduct(jobject Obj);
	static int GetTypeOfProduct(jobject Obj);
	static FString GetScoverOfProduct(jobject Obj);
	static FString GetBriefOfProduct(jobject Obj);
	static float GetPriceOfProduct(jobject Obj);

	static int LaunchCheckoutFlow(FString Sku, float Amount);
	static int ConsumePurchase(FString Sku);
	//Sport
	static int GetUseInfo();
	static int GetGender(jobject Obj);
	static int GetBirthDay(jobject Obj);
	static int GetStature(jobject Obj);
	static int GetWeight(jobject Obj);
	static int GetPlanCalorie(jobject Obj);
	static int GetPlanDurationInSeconds(jobject Obj);
	static int GetDaysPerWeek(jobject Obj);

	static int GetSummary(long beginTime, long endTime);
	static int GetSummary(FString beginTime, FString endTime);
	static long GetSummaryDurationInSeconds(jobject Obj);
	static double GetSummaryCalorie(jobject Obj);

	static int GetDailySummary(long beginTime, long endTime);
	static int GetDailySummary(FString beginTime, FString endTime);
	static int GetDailySummarySize(jobject Obj);
	static jobject GetDailySummaryByIndex(jobject Obj, int i);
	static FString GetDailySummaryDate(jobject Obj);
	static int GetDailySummaryPlanDurationInSeconds(jobject Obj);
	static long GetDailySummaryDurationInSeconds(jobject Obj);
	static double GetDailySummaryCalorie(jobject Obj);
	static double GetDailySummaryPlanCalorie(jobject Obj);
#endif
};