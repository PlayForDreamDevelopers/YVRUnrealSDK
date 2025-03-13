#include "YvrIdentityCallbackProxy.h"
#include "OnlineSubsystemYvrTypesPrivate.h"
#include "Online.h"

UYvrIdentityCallbackProxy::UYvrIdentityCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), LocalUserNum(0)
{
}

UYvrIdentityCallbackProxy* UYvrIdentityCallbackProxy::GetYvrIdentity(int32 LocalUserNum)
{
	UYvrIdentityCallbackProxy* Proxy = NewObject<UYvrIdentityCallbackProxy>();
	Proxy->LocalUserNum = LocalUserNum;
	Proxy->SetFlags(RF_StrongRefOnFrame);
	return Proxy;
}

void UYvrIdentityCallbackProxy::Activate()
{
	auto YvrIdentityInterface = Online::GetIdentityInterface(YVR_SUBSYSTEM);

	if (YvrIdentityInterface.IsValid())
	{
		DelegateHandle = YvrIdentityInterface->AddOnLoginCompleteDelegate_Handle(
			0,
			FOnLoginCompleteDelegate::CreateUObject(this, &UYvrIdentityCallbackProxy::OnLoginCompleteDelegate)
		);
		YvrIdentityInterface->AutoLogin(LocalUserNum);
	}
	else
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("Yvr platform service not available to get the Yvr ID."));
		OnFailure.Broadcast();
	}
}

void UYvrIdentityCallbackProxy::OnLoginCompleteDelegate(int32 Unused, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorStr)
{
	Online::GetIdentityInterface()->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, DelegateHandle);
	if (bWasSuccessful)
	{
		auto PlayerNickName = Online::GetIdentityInterface(YVR_SUBSYSTEM)->GetPlayerNickname(LocalUserNum);
		OnSuccess.Broadcast(UserId.ToString(), PlayerNickName);
	}
	else
	{
		OnFailure.Broadcast();
	}
}