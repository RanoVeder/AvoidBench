#include "AverageGoalVelocity.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

AAverageGoalVelocity::AAverageGoalVelocity()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AAverageGoalVelocity::Init(const FVector &start, const FVector &end)
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

void AAverageGoalVelocity::Start()
{
	Running = true;
}

void AAverageGoalVelocity::Stop()
{
	Running = false;
}

void AAverageGoalVelocity::Reset()
{
	ReferenceDistance = 0;
	MissionTime = 0;
}

TSharedPtr<FJsonObject> AAverageGoalVelocity::SerializeResults()
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
			Out->SetNumberField("AverageGoalVelocity", ReferenceDistance/MissionTime);
		}
	}
	else
	{
		Out->SetStringField("Error", TEXT("Failed to initialize the 'AverageGoalVelocity' Metric."));
	}
	return Out;
}

void AAverageGoalVelocity::BeginPlay()
{
	Super::BeginPlay();
}

void AAverageGoalVelocity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Running && IsInitialised)
	{
		MissionTime += DeltaTime;
	}
}
