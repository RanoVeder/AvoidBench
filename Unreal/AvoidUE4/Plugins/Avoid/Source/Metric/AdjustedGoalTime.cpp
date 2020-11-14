#include "AdjustedGoalTime.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

AAdjustedGoalTime::AAdjustedGoalTime()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AAdjustedGoalTime::Init(const FVector &start, const FVector &end)
{

	StartLocation = start;
	EndLocation = end;

	UOptimalPathCalculator *Calculator = NewObject<UOptimalPathCalculator>();
	AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GetWorld() == nullptr || Calculator->Init(GetWorld(), start, end) == false)
	{
		IsInitialised = false;
		return false;
	}

	ReferenceDistance = Calculator->GetPathLength() / 100.0;
	IsInitialised = true;
	return true;
}

void AAdjustedGoalTime::Start()
{
	Running = true;
}

void AAdjustedGoalTime::Stop()
{
	Running = false;
}

void AAdjustedGoalTime::Reset()
{
	ReferenceDistance = 0;
	MissionTime = 0;
}

TSharedPtr<FJsonObject> AAdjustedGoalTime::SerializeResults()
{
	TSharedPtr<FJsonObject> Out = MakeShareable(new FJsonObject);
	if (IsInitialised)
	{
		if (ReferenceDistance == 0)
		{
			Out->SetStringField("Error", TEXT("Could not calculate the reference distance"));
		}
		else
		{
			Out->SetNumberField("MissionTime", MissionTime);
			Out->SetNumberField("ReferenceDistance", ReferenceDistance);
			Out->SetNumberField("AdjustedGoalTime", MissionTime / ReferenceDistance);
		}
	}
	else
	{
		Out->SetStringField("Error", TEXT("Failed to initialize the 'AAdjustedGoalTime' Metric."));
	}
	return Out;
}

void AAdjustedGoalTime::BeginPlay()
{
	Super::BeginPlay();
}

void AAdjustedGoalTime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Running && IsInitialised)
	{
		MissionTime += DeltaTime;
	}
}
