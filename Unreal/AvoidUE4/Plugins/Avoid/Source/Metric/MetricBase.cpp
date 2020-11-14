#include "MetricBase.h"

AMetricBase::AMetricBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AMetricBase::Init(const FVector &StartLocation, const FVector &EndLocation)
{
	this->StartLocation = StartLocation;
	this->EndLocation = EndLocation;
	return true;
}

void AMetricBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMetricBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
