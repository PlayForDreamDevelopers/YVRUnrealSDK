#include "YvrXRMRModule.h"
#include "YvrXRAnchorManager.h"
#include "YvrXRSpaceMeshManager.h"
#include "YvrXRMarkerManager.h"

#define LOCTEXT_NAMESPACE "FYvrXRMRModule"

FYvrXRMRModule::FYvrXRMRModule()
	: YvrXRHMD(nullptr)
{
}

FYvrXRMRModule::~FYvrXRMRModule()
{
}

void FYvrXRMRModule::StartupModule()
{
	if (GEngine)
	{
		if (GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == TEXT("YvrXRHMD")))
		{
			YvrXRHMD = static_cast<FYvrXRHMD*>(GEngine->XRSystem.Get());
		}
	}

	FYvrXRSpaceMeshManager::GetInstance()->Initialize(YvrXRHMD);
	FYvrAnchorManager::GetInstance()->Initialize(YvrXRHMD);
	FYvrXRMarkerManager::GetInstance()->Initialize(YvrXRHMD);

}

void FYvrXRMRModule::ShutdownModule()
{
	FYvrAnchorManager::GetInstance()->Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FYvrXRMRModule, YvrXRMR)