#include "Traversability.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

// Sets default values
ATraversability::ATraversability()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Component"));
}

bool ATraversability::Init(const FVector &start, const FVector &end)
{

	//StartLocation = start;
	//EndLocation = end;

	//UOptimalPathCalculator* calculator = NewObject<UOptimalPathCalculator>();
	//AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//if (GetWorld() == nullptr || calculator->Init(GetWorld(), start, end) == false)
	//{
	//	IsInitialised = false;
	//	return false;
	//}

	//DroneActor = GameMode->DroneManager->GetActor();
	//if (DroneActor == nullptr)
	//{
	//	IsInitialised = false;
	//	return false;
	//}

	//ReferenceDistance = calculator->GetPathLength();
	//PreviousLocation = start;
	//IsInitialised = true;
	return true;
}

void ATraversability::SamplePosition()
{
	FHitResult Hit;
	float SampleTraversability = 0;

	FVector StartLocation = GetActorLocation();
	float AngleDelta = 2 * PI / (float)SamplesPerPosition;

	FlushPersistentDebugLines(GetWorld());

	for (int i = 0; i < SamplesPerPosition; i++)
	{
		FVector DirectionUnitVector{cos(AngleDelta * i), sin(AngleDelta * i), 0};
		FVector EndLocation = StartLocation + DirectionUnitVector * MaxRayLength;

		FCollisionQueryParams CollisionParameters;

		GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_WorldStatic, CollisionParameters);

		if (Hit.IsValidBlockingHit())
		{
			SampleTraversability += Hit.Distance;

			DrawDebugLine(GetWorld(), StartLocation, Hit.Location, FColor::Green, true, -1, 0, 1.f);
		}
		else
		{
			SampleTraversability += MaxRayLength;
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, -1, 0, 1.f);
		}
	}

	float res = (SampleTraversability / (float)SamplesPerPosition);
	UE_LOG(LogTemp, Warning, TEXT("Sample: %f"), res);
	return;
}

void ATraversability::Start()
{
	running = true;
}

void ATraversability::Stop()
{
	running = false;
}

void ATraversability::Reset()
{
	//TravelledDistance = 0;
	//PreviousLocation = StartLocation;
}

TSharedPtr<FJsonObject> ATraversability::SerializeResults()
{
	TSharedPtr<FJsonObject> out = MakeShareable(new FJsonObject);
	out->SetNumberField("Traversability", Traversability);
	return out;
}

// Called when the game starts or when spawned
void ATraversability::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATraversability::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (running)
	{
		/*	if (IsInitialised)
		{
			
		}*/
	}
}
