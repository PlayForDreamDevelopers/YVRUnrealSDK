#include "YvrXRHMD_DynamicResolutionState.h"
#include "LegacyScreenPercentageDriver.h"
#include "SceneView.h"

//-------------------------------------------------------------------------------------------------
// FScreenPercentage
//-------------------------------------------------------------------------------------------------

#if ENGINE_MAJOR_VERSION < 5

class FScreenPercentage : public ISceneViewFamilyScreenPercentage
{
public:
	FScreenPercentage(const FSceneViewFamily& InViewFamily, float InResolutionFraction, float InResolutionFractionUpperBound);

protected:
	// ISceneViewFamilyScreenPercentage
	virtual float GetPrimaryResolutionFractionUpperBound() const override;
	virtual ISceneViewFamilyScreenPercentage* Fork_GameThread(const class FSceneViewFamily& ForkedViewFamily) const override;
	virtual void ComputePrimaryResolutionFractions_RenderThread(TArray<FSceneViewScreenPercentageConfig>& OutViewScreenPercentageConfigs) const override;

private:
	const FSceneViewFamily& ViewFamily;
	const float ResolutionFraction;
	const float ResolutionFractionUpperBound;
};


//-------------------------------------------------------------------------------------------------
// FScreenPercentage implementation
//-------------------------------------------------------------------------------------------------

FScreenPercentage::FScreenPercentage(const FSceneViewFamily& InViewFamily, float InResolutionFraction, float InResolutionFractionUpperBound)
	: ViewFamily(InViewFamily)
	, ResolutionFraction(InResolutionFraction)
	, ResolutionFractionUpperBound(InResolutionFractionUpperBound)
{
	check(ViewFamily.EngineShowFlags.ScreenPercentage == true);
}

float FScreenPercentage::GetPrimaryResolutionFractionUpperBound() const
{
	return ResolutionFractionUpperBound;
}

ISceneViewFamilyScreenPercentage* FScreenPercentage::Fork_GameThread(const class FSceneViewFamily& ForkedViewFamily) const
{
	return new FScreenPercentage(ForkedViewFamily, ResolutionFraction, ResolutionFractionUpperBound);
}

void FScreenPercentage::ComputePrimaryResolutionFractions_RenderThread(TArray<FSceneViewScreenPercentageConfig>& OutViewScreenPercentageConfigs) const
{
	check(IsInRenderingThread());
	check(ViewFamily.EngineShowFlags.ScreenPercentage == true);

	for (int32 ConfigIter = 0; ConfigIter < OutViewScreenPercentageConfigs.Num(); ++ConfigIter)
	{
		OutViewScreenPercentageConfigs[ConfigIter].PrimaryResolutionFraction = ResolutionFraction;
	}
}

//-------------------------------------------------------------------------------------------------
// FYvrXRDynamicResolutionState implementation
//-------------------------------------------------------------------------------------------------

FYvrXRDynamicResolutionState::FYvrXRDynamicResolutionState(UYvrXRHMDSettings* InSettings)
	: Settings(InSettings)
	, ResolutionFraction(-1.0f)
	, ResolutionFractionUpperBound(-1.0f)
{
}

void FYvrXRDynamicResolutionState::ResetHistory()
{
};

bool FYvrXRDynamicResolutionState::IsSupported() const
{
	return true;
}

void FYvrXRDynamicResolutionState::SetupMainViewFamily(class FSceneViewFamily& ViewFamily)
{
	check(IsInGameThread());
	check(ViewFamily.EngineShowFlags.ScreenPercentage == true);

	if (IsEnabled())
	{
		// Compute desired resolution fraction range
		float MinResolutionFraction = Settings->PixelDensityMin;
		float MaxResolutionFraction = Settings->PixelDensityMax;

		// Clamp resolution fraction to what the renderer can do.
		MinResolutionFraction = FMath::Max(MinResolutionFraction, FSceneViewScreenPercentageConfig::kMinResolutionFraction);
		MaxResolutionFraction = FMath::Min(MaxResolutionFraction, FSceneViewScreenPercentageConfig::kMaxResolutionFraction);

		ResolutionFraction = FMath::Clamp(Settings->PixelDensity, MinResolutionFraction, MaxResolutionFraction);
		ResolutionFractionUpperBound = MaxResolutionFraction;

		ViewFamily.SetScreenPercentageInterface(new FScreenPercentage(ViewFamily, ResolutionFraction, ResolutionFractionUpperBound));
	}
}

float FYvrXRDynamicResolutionState::GetResolutionFractionApproximation() const
{
	return ResolutionFraction;
}

float FYvrXRDynamicResolutionState::GetResolutionFractionUpperBound() const
{
	return ResolutionFractionUpperBound;
}

void FYvrXRDynamicResolutionState::SetEnabled(bool bEnable)
{
	check(IsInGameThread());
	Settings->bPixelDensityAdaptive = bEnable;
}

bool FYvrXRDynamicResolutionState::IsEnabled() const
{
	check(IsInGameThread());
	return Settings->bPixelDensityAdaptive;
}

void FYvrXRDynamicResolutionState::ProcessEvent(EDynamicResolutionStateEvent Event)
{
};

#endif