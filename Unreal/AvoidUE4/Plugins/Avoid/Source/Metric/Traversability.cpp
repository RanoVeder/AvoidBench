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
	AMetricBase::Init(start, end);
	FlushPersistentDebugLines(GetWorld());
	Traversability = 0;

	if (GetWorld() == nullptr || MeasureArea == nullptr)
	{
		IsInitialised = false;
		return false;
	}


	auto boundingBox = MeasureArea->GetComponentsBoundingBox();
	FVector boundingBoxSize = boundingBox.GetSize();

	FVector boundingBoxMax = boundingBox.Max;
	FVector boundingBoxMin = boundingBox.Min;
	
	int GridSamplesX = 1 + (boundingBoxMax.X - boundingBoxMin.X) / GridSize;
	int GridSamplesY = 1 + (boundingBoxMax.Y - boundingBoxMin.Y) / GridSize;
	int GridSamples = GridSamplesX * GridSamplesY;


	FVector SampleLocation;

	for (int x = 0; x < GridSamplesX; x++)
	{
		for (int y = 0; y < GridSamplesY; y++)
		{
			SampleLocation = boundingBoxMin + FVector(x * GridSize, y * GridSize, -1);
			Traversability += SampleIndividualLocation(SampleLocation, false);
		}
	}

	Traversability /= ( DroneDiameter * (float)GridSamples);
	IsInitialised = true;

	return true;
}


void ATraversability::CalculateTraversabilityDebug()
{
	FlushPersistentDebugLines(GetWorld());
	Traversability = 0;

	if (GetWorld() == nullptr || MeasureArea == nullptr)
	{
		IsInitialised = false;
		return;
	}


	auto boundingBox = MeasureArea->GetComponentsBoundingBox();
	FVector boundingBoxSize = boundingBox.GetSize();

	FVector boundingBoxMax = boundingBox.Max;
	FVector boundingBoxMin = boundingBox.Min;

	int GridSamplesX = 1 + (boundingBoxMax.X - boundingBoxMin.X) / GridSize;
	int GridSamplesY = 1 + (boundingBoxMax.Y - boundingBoxMin.Y) / GridSize;
	int GridSamples = GridSamplesX * GridSamplesY;


	FVector SampleLocation;

	for (int x = 0; x < GridSamplesX; x++)
	{
		for (int y = 0; y < GridSamplesY; y++)
		{
			SampleLocation = boundingBoxMin + FVector(x * GridSize, y * GridSize, 0);
			Traversability += SampleIndividualLocation(SampleLocation, true);
		}
	}

	Traversability /= (DroneDiameter * (float)GridSamples);

	UE_LOG(LogTemp, Warning, TEXT("Traversability: %f"), Traversability);


}

void ATraversability::ClearDebugLines()
{
	FlushPersistentDebugLines(GetWorld());
}


float ATraversability::SampleIndividualLocation(const FVector& Position, bool Debug)
{
	FHitResult Hit;
	float SampleTraversability = 0;

	FVector StartLocation = Position;
	float AngleDelta = 2 * PI / (float)SamplesPerPosition;

	

	for (int i = 0; i < SamplesPerPosition; i++)
	{
		FVector DirectionUnitVector{cos(AngleDelta * i), sin(AngleDelta * i), 0};
		FVector EndLocation = StartLocation + DirectionUnitVector * MaxRayLength;

		FCollisionQueryParams CollisionParameters;
		CollisionParameters.bTraceComplex = true;

		GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_WorldDynamic, CollisionParameters);

		if (Hit.IsValidBlockingHit())
		{
			SampleTraversability += Hit.Distance;

			if(Debug)
				DrawDebugLine(GetWorld(), StartLocation, Hit.Location, FColor::Red, true, -1, 0, 1.f);
		}
		else
		{
			SampleTraversability += MaxRayLength;
			if (Debug)
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, -1, 0, 1.f);
		}
	}

	float res = (SampleTraversability / (float)SamplesPerPosition);
	return res;
}

void ATraversability::Start()
{
}

void ATraversability::Stop()
{
}

void ATraversability::Reset()
{
	Traversability = 0;
	//PreviousLocation = StartLocation;
}

TSharedPtr<FJsonObject> ATraversability::SerializeResults()
{
	TSharedPtr<FJsonObject> out = MakeShareable(new FJsonObject);
	if (IsInitialised)
	{
		out->SetNumberField("Traversability", Traversability);
		//Convert to metric
		out->SetNumberField("Drone Diameter", DroneDiameter/100.0);
	}
	else
	{
		out->SetStringField("Error", "Failed to initialise the Traversability Metric");
	}
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

}
