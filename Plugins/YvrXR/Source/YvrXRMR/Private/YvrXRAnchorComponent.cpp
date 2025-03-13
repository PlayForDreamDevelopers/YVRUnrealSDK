#include "YvrXRAnchorComponent.h"
#include "YvrXRAnchorManager.h"

UYvrAnchorComponent::UYvrAnchorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AnchorHandle = 0;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UYvrAnchorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UYvrAnchorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(GetOwner()) && IsValid(GetWorld()))
	{

		AActor* Parent = GetOwner();
		FYvrAnchorManager::GetInstance()->UpdateAnchor(this);
	}
}

void UYvrAnchorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}