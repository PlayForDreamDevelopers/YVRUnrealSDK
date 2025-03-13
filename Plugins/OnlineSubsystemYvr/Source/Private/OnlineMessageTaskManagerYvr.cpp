#include "OnlineMessageTaskManagerYvr.h"
#include "OnlineSubsystemYvrWrapper.h"
#include "OnlineSubsystem.h"

void FOnlineMessageTaskManagerYvr::OnReceiveMessage(YvrMessageHandle Message)
{
#if PLATFORM_ANDROID
	auto RequestId = OnlineSubsystemYvrWrapper::GetRequestId(Message);
	bool bIsError = OnlineSubsystemYvrWrapper::IsMessageError(Message);

	if (RequestDelegates.Contains(RequestId))
	{
		RequestDelegates[RequestId].ExecuteIfBound(Message, bIsError);

		// Remove the delegate
		RequestDelegates[RequestId].Unbind();
		RequestDelegates.Remove(RequestId);
	}
	else
	{
		auto MessageType = OnlineSubsystemYvrWrapper::GetRequestType(Message);
		if (NotifyDelegates.Contains(MessageType))
		{
			if (!bIsError)
			{
				NotifyDelegates[MessageType].Broadcast(Message, bIsError);
			}
		}
		else
		{
			UE_LOG_ONLINE(Verbose, TEXT("Unhandled request id: %llu type: %s"), RequestId, *MessageType);
		}
	}
	OnlineSubsystemYvrWrapper::FreeRequest(RequestId);
#endif
}

void FOnlineMessageTaskManagerYvr::AddRequestDelegate(uint64 RequestId, FYvrMessageOnCompleteDelegate&& Delegate)
{
	RequestDelegates.Emplace(RequestId, Delegate);
}

FYvrMulticastMessageOnCompleteDelegate& FOnlineMessageTaskManagerYvr::GetNotifyDelegate(YvrMessageType MessageType)
{
	return NotifyDelegates.FindOrAdd(MessageType);
}

void FOnlineMessageTaskManagerYvr::RemoveNotifyDelegate(YvrMessageType MessageType, const FDelegateHandle& Delegate)
{
	NotifyDelegates.FindOrAdd(MessageType).Remove(Delegate);
}

bool FOnlineMessageTaskManagerYvr::Tick(float DeltaTime)
{
#if PLATFORM_ANDROID
	for (;;)
	{
		auto Message = OnlineSubsystemYvrWrapper::PopMessage();
		if (!Message)
		{
			break;
		}
		OnReceiveMessage(Message);
	}
#endif

	if (DeltaTime > 4.0f)
	{
		UE_LOG_ONLINE(Warning, TEXT("DeltaTime was %f seconds.  Time sensitive Yvr notifications may time out."), DeltaTime);
	}
	return true;
}