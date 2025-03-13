// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystemYvr.h"
#include "OnlineSubsystemYvrTypesPrivate.h"
#include "Interfaces/OnlineIdentityInterface.h"

class FUserOnlineAccountYvr :
	public FUserOnlineAccount
{

public:

	// FOnlineUser
	virtual TSharedRef<const FUniqueNetId> GetUserId() const override { return UserIdPtr; }
	virtual FString GetRealName() const override { return Name; }
	virtual FString GetDisplayName(const FString& Platform = FString()) const override { return Name; }
	virtual bool GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const override;
	virtual bool SetUserAttribute(const FString& AttrName, const FString& AttrValue) override;

	// FUserOnlineAccount
	virtual FString GetAccessToken() const override { return TEXT("ACCESSTOKEN"); }
	virtual bool GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const override;

	// FUserOnlineAccountYvr
	FUserOnlineAccountYvr(const TSharedRef<const FUniqueNetId>& InUserId, const FString& InName)
		: UserIdPtr(InUserId),
		Name(InName)
	{ }

	virtual ~FUserOnlineAccountYvr()
	{
	}

	TSharedRef<const FUniqueNetId> UserIdPtr;
	TMap<FString, FString> AdditionalAuthData;
	TMap<FString, FString> UserAttributes;

private:
	FString Name;
};

class FOnlineIdentityYvr : public IOnlineIdentity
{
public:

	// IOnlineIdentity
	virtual bool Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials) override;
	virtual bool Logout(int32 LocalUserNum) override;
	virtual bool AutoLogin(int32 LocalUserNum) override;
	virtual TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId& UserId) const override;
	virtual TArray<TSharedPtr<FUserOnlineAccount> > GetAllUserAccounts() const override;
	virtual TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
	virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(uint8* Bytes, int32 Size) override;
	virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(const FString& Str) override;
	virtual ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const override;
	virtual ELoginStatus::Type GetLoginStatus(const FUniqueNetId& UserId) const override;
	virtual FString GetPlayerNickname(int32 LocalUserNum) const override;
	virtual FString GetPlayerNickname(const FUniqueNetId& UserId) const override;
	virtual FString GetAuthToken(int32 LocalUserNum) const override;
	virtual void RevokeAuthToken(const FUniqueNetId& UserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate) override;
	virtual void GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate, EShowPrivilegeResolveUI ShowResolveUI) override;
	virtual FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const override;
	virtual FString GetAuthType() const override;

	// FOnlineIdentityYvr
	FOnlineIdentityYvr(FOnlineSubsystemYvr& InSubsystem);

	virtual ~FOnlineIdentityYvr() = default;

PACKAGE_SCOPE:

	void OnLoginComplete(YvrMessageHandle Message, bool bIsError, int32 LocalUserNum);

private:

	FOnlineSubsystemYvr& YvrSubsystem;
	TMap<int32, TSharedPtr<const FUniqueNetId>> UserIds;
	TMap<FUniqueNetIdYvr, TSharedRef<FUserOnlineAccountYvr>> UserAccounts;
};

typedef TSharedPtr<FOnlineIdentityYvr, ESPMode::ThreadSafe> FOnlineIdentityYvrPtr;
