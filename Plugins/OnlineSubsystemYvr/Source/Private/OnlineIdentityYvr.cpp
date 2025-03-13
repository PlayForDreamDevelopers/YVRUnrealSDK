#include "OnlineIdentityYvr.h"
#include "OnlineSubsystemYvrWrapper.h"
#include "OnlineError.h"

bool FUserOnlineAccountYvr::GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	const FString* FoundAttr = AdditionalAuthData.Find(AttrName);
	if (FoundAttr != nullptr)
	{
		OutAttrValue = *FoundAttr;
		return true;
	}
	return false;
}

bool FUserOnlineAccountYvr::SetUserAttribute(const FString& AttrName, const FString& AttrValue)
{
	UserAttributes[AttrName] = AttrValue;
	return true;
}

bool FUserOnlineAccountYvr::GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	const FString* FoundAttr = UserAttributes.Find(AttrName);
	if (FoundAttr != nullptr)
	{
		OutAttrValue = *FoundAttr;
		return true;
	}
	return false;
}

bool FOnlineIdentityYvr::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
#if PLATFORM_ANDROID
	FString ErrorStr;
	TSharedPtr<FUserOnlineAccountYvr> UserAccountPtr;

	if (LocalUserNum < 0 || LocalUserNum >= MAX_LOCAL_PLAYERS)
	{
		ErrorStr = FString::Printf(TEXT("Invalid LocalUserNum=%d"), LocalUserNum);
	}
	else
	{
		TSharedPtr<const FUniqueNetId>* UserId = UserIds.Find(LocalUserNum);
		if (UserId == nullptr)
		{
			YvrSubsystem.AddRequestDelegate(
				OnlineSubsystemYvrWrapper::GetLoggedInUser(),
				FYvrMessageOnCompleteDelegate::CreateRaw(this, &FOnlineIdentityYvr::OnLoginComplete, LocalUserNum));
			return true;
		}
		else
		{
			TriggerOnLoginCompleteDelegates(LocalUserNum, true, *UserId->Get(), *ErrorStr);
		}
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("Failed Yvr login. %s"), *ErrorStr);
		TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdYvr(), ErrorStr);
	}

#endif
	return false;
}

void FOnlineIdentityYvr::OnLoginComplete(YvrMessageHandle Message, bool bIsError, int32 LocalUserNum)
{
#if PLATFORM_ANDROID
	FString ErrorStr;
	if (bIsError)
	{
		auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
		ErrorStr = FString(ErrorMessage);
	}
	else
	{
		auto Id = OnlineSubsystemYvrWrapper::GetAccountId(Message);
		FString Name(OnlineSubsystemYvrWrapper::GetUserName(Message));

		TSharedPtr<const FUniqueNetId>* NewUserId = UserIds.Find(LocalUserNum);
		if (NewUserId == nullptr || !NewUserId->IsValid() || static_cast<const FUniqueNetIdYvr>(*NewUserId->Get()).GetID() != Id)
		{
			UserIds.Add(LocalUserNum, MakeShareable(new FUniqueNetIdYvr(Id)));
			NewUserId = UserIds.Find(LocalUserNum);
		}

		if (!NewUserId->IsValid())
		{
			ErrorStr = FString(TEXT("Unable to get a valid ID"));
		}
		else
		{
			TSharedRef<FUserOnlineAccountYvr> UserAccountRef(new FUserOnlineAccountYvr(NewUserId->ToSharedRef(), Name));

			// update/add cached entry for user
			UserAccounts.Add(static_cast<FUniqueNetIdYvr>(*UserAccountRef->GetUserId()), UserAccountRef);

			TriggerOnLoginCompleteDelegates(LocalUserNum, true, *UserAccountRef->GetUserId(), *ErrorStr);
			TriggerOnLoginStatusChangedDelegates(LocalUserNum, ELoginStatus::NotLoggedIn, ELoginStatus::LoggedIn, *UserAccountRef->GetUserId());
			return;
		}
	}

	TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdYvr(), *ErrorStr);
#endif
}

bool FOnlineIdentityYvr::Logout(int32 LocalUserNum)
{
	TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		UserAccounts.Remove(FUniqueNetIdYvr(*UserId));
		UserIds.Remove(LocalUserNum);
		TriggerOnLogoutCompleteDelegates(LocalUserNum, true);
		TriggerOnLoginStatusChangedDelegates(LocalUserNum, ELoginStatus::LoggedIn, ELoginStatus::NotLoggedIn, *UserId);
		return true;
	}
	else
	{
		TriggerOnLogoutCompleteDelegates(LocalUserNum, false);
	}
	return false;
}

bool FOnlineIdentityYvr::AutoLogin(int32 LocalUserNum)
{
	FString LoginStr;
	FString PasswordStr;
	FString TypeStr;

	return Login(LocalUserNum, FOnlineAccountCredentials(TypeStr, LoginStr, PasswordStr));
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityYvr::GetUserAccount(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> Result;

	FUniqueNetIdYvr YvrUserId(UserId);
	const TSharedRef<FUserOnlineAccountYvr>* FoundUserAccount = UserAccounts.Find(YvrUserId);
	if (FoundUserAccount != nullptr)
	{
		Result = *FoundUserAccount;
	}

	return Result;
}

TArray<TSharedPtr<FUserOnlineAccount>> FOnlineIdentityYvr::GetAllUserAccounts() const
{
	TArray<TSharedPtr<FUserOnlineAccount> > Result;

	for (TMap<FUniqueNetIdYvr, TSharedRef<FUserOnlineAccountYvr>>::TConstIterator It(UserAccounts); It; ++It)
	{
		Result.Add(It.Value());
	}

	return Result;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityYvr::GetUniquePlayerId(int32 LocalUserNum) const
{
	const TSharedPtr<const FUniqueNetId>* FoundId = UserIds.Find(LocalUserNum);
	if (FoundId != nullptr)
	{
		return *FoundId;
	}
	return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityYvr::CreateUniquePlayerId(uint8* Bytes, int32 Size)
{
	if (Bytes && Size == sizeof(int))
	{
		uint64* RawUniqueId = (uint64*)Bytes;
		int YvrId(*RawUniqueId);
		return MakeShareable(new FUniqueNetIdYvr(YvrId));
	}
	return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityYvr::CreateUniquePlayerId(const FString& Str)
{
	return MakeShareable(new FUniqueNetIdYvr(Str));
}

ELoginStatus::Type FOnlineIdentityYvr::GetLoginStatus(int32 LocalUserNum) const
{
	TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		return GetLoginStatus(*UserId);
	}
	return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityYvr::GetLoginStatus(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(UserId);
	if (UserAccount.IsValid() &&
		UserAccount->GetUserId()->IsValid())
	{
		return ELoginStatus::LoggedIn;
	}
	return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityYvr::GetPlayerNickname(int32 LocalUserNum) const
{
	TSharedPtr<const FUniqueNetId> UniqueId = GetUniquePlayerId(LocalUserNum);
	if (UniqueId.IsValid())
	{
		return GetPlayerNickname(*UniqueId);
	}

	return TEXT("YVR USER");
}

FString FOnlineIdentityYvr::GetPlayerNickname(const FUniqueNetId& UserId) const
{
	auto UserAccount = GetUserAccount(UserId);
	if (UserAccount.IsValid())
	{
		return UserAccount->GetDisplayName();
	}
	return UserId.ToString();
}

FString FOnlineIdentityYvr::GetAuthToken(int32 LocalUserNum) const
{
	return FString();
}

void FOnlineIdentityYvr::RevokeAuthToken(const FUniqueNetId& UserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate)
{
	UE_LOG_ONLINE_IDENTITY(Display, TEXT("FOnlineIdentityYvr::RevokeAuthToken not implemented"));
	TSharedRef<const FUniqueNetId> UserIdRef(UserId.AsShared());
	YvrSubsystem.ExecuteNextTick([UserIdRef, Delegate]()
	{
		Delegate.ExecuteIfBound(*UserIdRef, FOnlineError(FString(TEXT("RevokeAuthToken not implemented"))));
	});
}

void FOnlineIdentityYvr::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate, EShowPrivilegeResolveUI ShowResolveUI = EShowPrivilegeResolveUI::Default)
{
#if PLATFORM_ANDROID
	// Check for entitlement
	YvrSubsystem.AddRequestDelegate(
		OnlineSubsystemYvrWrapper::GetViewerEntitled(),
		FYvrMessageOnCompleteDelegate::CreateLambda([&UserId, Privilege, Delegate](YvrMessageHandle Message, bool bIsError)
			{
				uint32 PrivilegeResults = 0;

				if (bIsError)
				{
					FString ErrorMessage(OnlineSubsystemYvrWrapper::GetErrorMessage(Message));
					UE_LOG_ONLINE_IDENTITY(Error, TEXT("Failed the entitlement check: %s"), *ErrorMessage);
					PrivilegeResults = static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::UserNotFound);
				}
				else
				{
					bool bIsEntitled = OnlineSubsystemYvrWrapper::IsViewerEntitled(Message);
					if (bIsEntitled)
					{
						UE_LOG_ONLINE_IDENTITY(Verbose, TEXT("User is entitled to app"));
						PrivilegeResults = static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::NoFailures);
					}
					else
					{
						UE_LOG_ONLINE_IDENTITY(Error, TEXT("Failed the entitlement check"));
						PrivilegeResults = static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::UserNotFound);
					}
				}
				Delegate.ExecuteIfBound(UserId, Privilege, PrivilegeResults);
			}));
#endif
}

FPlatformUserId FOnlineIdentityYvr::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const
{
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		auto CurrentUniqueId = GetUniquePlayerId(i);
		if (CurrentUniqueId.IsValid() && (*CurrentUniqueId == UniqueNetId))
		{
			return GetPlatformUserIdFromLocalUserNum(i);
		}
	}

	return PLATFORMUSERID_NONE;
}

FString FOnlineIdentityYvr::GetAuthType() const
{
	return TEXT("Yvr");
}

FOnlineIdentityYvr::FOnlineIdentityYvr(FOnlineSubsystemYvr& InSubsystem)
	: YvrSubsystem(InSubsystem)
{
	FOnlineIdentityYvr::AutoLogin(0);
}