#include "OptimalityFactor.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"

AOptimalityFactor::AOptimalityFactor()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AOptimalityFactor::Init(const FVector &start, const FVector &end)
{
	AMetricBase::Init(start, end);

	UOptimalPathCalculator *calculator = NewObject<UOptimalPathCalculator>();
	AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GetWorld() == nullptr || calculator->Init(GetWorld(), StartLocation, EndLocation) == false)
	{
		IsInitialised = false;
		return false;
	}

	DroneActor = GameMode->DroneManager->GetActor();
	if (DroneActor == nullptr)
	{
		IsInitialised = false;
		return false;
	}

	ReferenceDistance = calculator->GetPathLength();
	PreviousLocation = start;
	IsInitialised = true;
	return true;
}

float AOptimalityFactor::GetOptimalityFactor()
{
	if (IsInitialised)
	{
		return TravelledDistance / ReferenceDistance;
	}
	else
	{
		return -1;
	}
}

void AOptimalityFactor::Start()
{
	running = true;
}

void AOptimalityFactor::Stop()
{
	running = false;
}

void AOptimalityFactor::Reset()
{
	TravelledDistance = 0;
	PreviousLocation = StartLocation;
}

TSharedPtr<FJsonObject> AOptimalityFactor::SerializeResults()
{
	TSharedPtr<FJsonObject> out = MakeShareable(new FJsonObject);
	out->SetNumberField("OptimalDistance", ReferenceDistance);
	out->SetNumberField("TravelledDistance", TravelledDistance);
	out->SetNumberField("OptimalityFactor", GetOptimalityFactor());
	return out;
}

void AOptimalityFactor::BeginPlay()
{
	Super::BeginPlay();
}

void AOptimalityFactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (running)
	{
		if (IsInitialised)
		{
			FVector CurrentLocation = DroneActor->GetActorLocation();
			float difference = (PreviousLocation - CurrentLocation).Size();
			TravelledDistance += difference;
			PreviousLocation = CurrentLocation;
		}
	}
}
