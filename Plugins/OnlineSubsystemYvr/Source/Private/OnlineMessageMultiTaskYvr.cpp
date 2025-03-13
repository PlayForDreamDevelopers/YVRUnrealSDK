#include "OnlineMessageMultiTaskYvr.h"

void FOnlineMessageMultiTaskYvr::AddNewRequest(int RequestId)
{
	InProgressRequests.Add(RequestId);
	YvrSubsystem.AddRequestDelegate(
		RequestId,
		FYvrMessageOnCompleteDelegate::CreateLambda([this, RequestId](YvrMessageHandle Message, bool bIsError)
	{
		InProgressRequests.Remove(RequestId);
		if (bIsError)
		{
			bDidAllRequestsFinishedSuccessfully = false;
		}

		if (InProgressRequests.Num() == 0)
		{
			Delegate.ExecuteIfBound();
		}
	}));
}