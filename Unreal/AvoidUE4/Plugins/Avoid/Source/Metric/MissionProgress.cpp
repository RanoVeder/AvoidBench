#include "MissionProgress.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

// Sets default values
AMissionProgress::AMissionProgress()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Component"));
}

bool AMissionProgress::Init(const FVector &start, const FVector &end)
{

	AMetricBase::Init(start, end);
	CurrentPosition = start;

	AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
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

	AAvoidMission* CurrentMission = GameMode->MissionManager->GetCurrentMission();
	if (CurrentMission == nullptr)
	{
		IsInitialised = false;
		return false;
	}

	GoalRadius = CurrentMission->GoalRadius;



	IsInitialised = true;
	Running = false;
	return true;
}


void AMissionProgress::Start()
{
	Running = true;
}

void AMissionProgress::Stop()
{
	Running = false;
}

void AMissionProgress::Reset()
{
	CurrentPosition = StartLocation;
}

TSharedPtr<FJsonObject> AMissionProgress::SerializeResults()
{
	TSharedPtr<FJsonObject> out = MakeShareable(new FJsonObject);
	if (IsInitialised)
	{
		out->SetNumberField("MissionProgress", GetMissionProgress());
	}
	else
	{
		out->SetStringField("Error", "Failed to initialise the MissionProgress Metric");
	}
	return out;
}

float AMissionProgress::GetMissionProgress()
{
	FVector GoalVector = EndLocation - StartLocation;
	FVector ProgressVector = CurrentPosition - StartLocation;

	FVector2D GoalVector2D(GoalVector.X, GoalVector.Y);
	FVector2D ProgressVector2D(ProgressVector.X, ProgressVector.Y);

	FVector GoalDifference = CurrentPosition - EndLocation;

	if (GoalDifference.Size2D() <= GoalRadius*1.1)
	{
		return 1.0;
	}
	else
	{
		//Vector Projection + Scaling from 0-1
		return FVector2D::DotProduct(GoalVector2D, ProgressVector2D) / GoalVector2D.SizeSquared();
	}
}

// Called when the game starts or when spawned
void AMissionProgress::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMissionProgress::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Running)
	{
		if (IsInitialised)
		{
			CurrentPosition = DroneActor->GetActorLocation();
		}
	}

}
