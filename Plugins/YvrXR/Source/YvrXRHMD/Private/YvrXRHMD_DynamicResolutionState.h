// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "YvrXRHMDSettings.h"
#include "DynamicResolutionState.h"


//-------------------------------------------------------------------------------------------------
// FYvrXRDynamicResolutionState
//-------------------------------------------------------------------------------------------------
#if ENGINE_MAJOR_VERSION < 5

class FYvrXRDynamicResolutionState : public IDynamicResolutionState
{
public:
	FYvrXRDynamicResolutionState(UYvrXRHMDSettings* InSettings);

	// ISceneViewFamilyScreenPercentage
	virtual void ResetHistory() override;
	virtual bool IsSupported() const override;
	virtual void SetupMainViewFamily(class FSceneViewFamily& ViewFamily) override;

protected:
	virtual float GetResolutionFractionApproximation() const override;
	virtual float GetResolutionFractionUpperBound() const override;
	virtual void SetEnabled(bool bEnable) override;
	virtual bool IsEnabled() const override;
	virtual void ProcessEvent(EDynamicResolutionStateEvent Event) override;

private:
	UYvrXRHMDSettings* Settings;
	float ResolutionFraction;
	float ResolutionFractionUpperBound;
};

#endif
