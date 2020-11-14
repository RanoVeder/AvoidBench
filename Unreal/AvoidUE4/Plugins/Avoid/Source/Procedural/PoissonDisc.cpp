// Fill out your copyright notice in the Description page of Project Settings.

#include "PoissonDisc.h"
#include "Components/BrushComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "DrawDebugHelpers.h"
// Sets default values
APoissonDisc::APoissonDisc()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Component"));

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APoissonDisc::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APoissonDisc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APoissonDisc::GenerateArray()
{
	if (volume == nullptr)
		return false;
	boundingBox = volume->GetComponentsBoundingBox();
	FVector boundingBoxSize = boundingBox.GetSize();

	gridSize = radius / FMath::Sqrt(2);
	rows = FMath::CeilToInt(boundingBoxSize.X / gridSize);
	columns = FMath::CeilToInt(boundingBoxSize.Y / gridSize);

	if (rows == 0 || columns == 0)
		return false;

	grid.Init(-1, rows * columns);
	locations.Empty(rows * columns);
	return true;
}

void APoissonDisc::GenerateLocations()
{
	FVector boundingBoxMax = boundingBox.Max;
	FVector boundingBoxMin = boundingBox.Min;

	FVector2D initial;
	initial.X = FMath::RandRange(boundingBoxMin.X, boundingBoxMax.X);
	initial.Y = FMath::RandRange(boundingBoxMin.Y, boundingBoxMax.Y);

	int row, column;
	GetGridBox(initial, row, column);

	locations.Add(initial);
	activePoints.Add(initial);
	grid[row * columns + column] = 0;

	while (activePoints.Num() != 0)
	{
		int index = FMath::RandRange(0, activePoints.Num() - 1);
		FVector2D currentPoint = activePoints[index];

		bool foundCandidate = false;

		for (int i = 0; i < iterations; i++)
		{

			FVector2D randomInCircle = FMath::RandPointInCircle(radius);
			FVector2D candidate = currentPoint + (randomInCircle + FMath::RandRange(0.0f, 1.0f) * (randomInCircle * 2 - randomInCircle));

			if (IsValid(candidate))
			{
				locations.Add(candidate);
				activePoints.Add(candidate);
				GetGridBox(candidate, row, column);
				grid[row * columns + column] = locations.Num() - 1;
				foundCandidate = true;
				break;
			}
		}

		if (!foundCandidate)
		{
			activePoints.RemoveAt(index);
		}
	}
}

void APoissonDisc::PlaceInstances()
{
	//FPotentialInstance PotentialInstance = PotentialInstances[Idx];/*;*/
	TArray<FDesiredFoliageInstance> desiredInstances;
	desiredInstances.Reserve(locations.Num());

	UWorld *world = GetWorld();
	if (world == nullptr)
		return;

	ULevel *level = world->GetCurrentLevel();
	if (level == nullptr)
		return;

	for (auto &point : locations)
	{

		FDesiredFoliageInstance desiredInstance;

		int index = FMath::RandRange(0, FoliageTypes.Num() - 1);
		FVector StartTrace = FVector(point, boundingBox.Max.Z);
		FVector EndTrace = FVector(point, -1e6);

		desiredInstance.FoliageType = FoliageTypes[index];
		desiredInstance.Rotation = FQuat(0, 0, 0, 1);
		desiredInstance.StartTrace = StartTrace;
		desiredInstance.EndTrace = EndTrace;
		desiredInstance.Age = 0;
		desiredInstance.TraceRadius = 10;

		desiredInstances.Add(desiredInstance);

		FHitResult Hit;

		if (!FoliageTrace(world, Hit, desiredInstance))
		{
			continue;
		}
		//FPotentialInstance potentialInstance = FPotentialInstance(Hit.ImpactPoint, Hit.ImpactNormal, Hit.GetComponent(), 1.0f, desiredInstance);
		FFoliageInstance Inst;
		if (PlaceInstance(world, desiredInstance.FoliageType, Inst, Hit.ImpactPoint, Hit.ImpactNormal))
		{
			UFoliageType_InstancedStaticMesh *cl = FoliageTypes[index];
			UFoliageInstancedStaticMeshComponent *component = instances[index];
			FTransform transform = Inst.GetInstanceWorldTransform();
			component->AddInstanceWorldSpace(transform);
			//UClass* aaa = cl->GetComponentClass();

			//UFoliageInstancedStaticMeshComponent* comp = aaa->GetDefaultObject< UFoliageInstancedStaticMeshComponent>();
			//comp->AddInstanceWorldSpace(transform);
		};
	};
}

bool APoissonDisc::IsValid(FVector2D point)
{
	FVector boundingBoxMax = boundingBox.Max;
	FVector boundingBoxMin = boundingBox.Min;

	int row, column;

	bool isValid = false;

	if (boundingBox.IsInsideXY(FVector(point, 0)))
	{
		GetGridBox(point, row, column);

		int startX = FMath::Max(0, row - 2);
		int endX = FMath::Min(rows - 1, row + 2);
		int startY = FMath::Max(0, column - 2);
		int endY = FMath::Min(columns - 1, column + 2);

		for (int x = startX; x <= endX; x++)
		{
			for (int y = startY; y <= endY; y++)
			{
				int comparisonIndex = grid[columns * x + y];
				if (comparisonIndex == -1)
				{
					continue;
				}
				FVector2D comparisonPoint = locations[comparisonIndex];
				float distance = (comparisonPoint - point).SizeSquared();
				if (distance < (radius * radius))
				{
					return false;
				}
			}
		}

		return true;
	}
	return false;
}

void APoissonDisc::GetGridBox(FVector2D location, int &row, int &column)
{
	FVector boundingBoxMin = boundingBox.Min;
	row = FMath::FloorToInt((location.X - boundingBoxMin.X) / gridSize);
	column = FMath::FloorToInt((location.Y - boundingBoxMin.Y) / gridSize);
}

void APoissonDisc::Generate()
{
	if (FoliageTypes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Foliages Specified"))
		return;
	};

	if (volume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Volume Specified"))
		return;
	};
	UE_LOG(LogTemp, Warning, TEXT("RUNNING"))
	FMath::RandInit(randomSeed);

	if (oldTypes != FoliageTypes)
	{
		oldTypes = FoliageTypes;

		for (auto instance : instances)
		{
			instance->UnregisterComponent();
			instance->DestroyComponent();
		};

		UWorld *world = GetWorld();
		if (world == nullptr)
			return;

		for (auto type : FoliageTypes)
		{
			FString name;
			type->GetName(name);
			FName componentName = FName(name + FString("_instance"));
			UFoliageInstancedStaticMeshComponent *component = NewObject<UFoliageInstancedStaticMeshComponent>(this, componentName);
			component->RegisterComponent();
			component->SetStaticMesh(type->GetStaticMesh());
			component->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			instances.Add(component);
			//UClass* aaa = type->GetComponentClass();

			//UFoliageInstancedStaticMeshComponent* comp = aaa->GetDefaultObject< UFoliageInstancedStaticMeshComponent>();
			//comp->RegisterComponentWithWorld(world);
		}
	}

	for (UFoliageInstancedStaticMeshComponent *instance : instances)
	{
		instance->ClearInstances();
	};

	if (GenerateArray())
	{
		GenerateLocations();
		PlaceInstances();
	}

	UE_LOG(LogTemp, Warning, TEXT("Done"))
};

bool APoissonDisc::FoliageTrace(const UWorld *InWorld, FHitResult &OutHit, const FDesiredFoliageInstance &DesiredInstance)
{
	FCollisionQueryParams QueryParams(FName(TEXT("AddFoliageInstances")), SCENE_QUERY_STAT_ONLY(IFA_FoliageTrace), true);
	QueryParams.bReturnFaceIndex = true;

	//It's possible that with the radius of the shape we will end up with an initial overlap which would place the instance at the top of the procedural volume.
	//Moving the start trace back a bit will fix this, but it introduces the potential for spawning instances a bit above the volume. This second issue is already somewhat broken because of how sweeps work so it's not too bad, also this is a less common case.
	//The proper fix would be to do something like EncroachmentCheck where we first do a sweep, then we fix it up if it's overlapping, then check the filters. This is more expensive and error prone so for now we just move the trace up a bit.
	const FVector Dir = (DesiredInstance.EndTrace - DesiredInstance.StartTrace).GetSafeNormal();
	const FVector StartTrace = DesiredInstance.StartTrace - (Dir * DesiredInstance.TraceRadius);

	TArray<FHitResult> Hits;
	FCollisionShape SphereShape;
	SphereShape.SetSphere(DesiredInstance.TraceRadius);
	InWorld->SweepMultiByObjectType(Hits, StartTrace, DesiredInstance.EndTrace, FQuat::Identity, FCollisionObjectQueryParams(ECC_WorldStatic), SphereShape, QueryParams);

	for (const FHitResult &Hit : Hits)
	{
		const UPrimitiveComponent *HitComponent = Hit.GetComponent();
		check(HitComponent);

		// In the editor traces can hit "No Collision" type actors, so ugh. (ignore these)
		if (!HitComponent->IsQueryCollisionEnabled() || HitComponent->GetCollisionResponseToChannel(ECC_WorldStatic) != ECR_Block)
		{
			continue;
		}

		// Don't place foliage on invisible walls / triggers / volumes
		if (HitComponent->IsA<UBrushComponent>())
		{
			continue;
		}

		OutHit = Hit;
		return true;
	}

	return false;
};

bool APoissonDisc::PlaceInstance(const UWorld *InWorld, const UFoliageType *Settings, FFoliageInstance &Inst, const FVector &HitLocation, const FVector &HitNormal)
{

	Inst.DrawScale3D = Settings->GetRandomScale();

	Inst.ZOffset = Settings->ZOffset.Interpolate(FMath::FRand());

	Inst.Location = HitLocation;

	// Random yaw and optional random pitch up to the maximum
	Inst.Rotation = FRotator(FMath::FRand() * Settings->RandomPitchAngle, 0.f, 0.f);

	Inst.Rotation.Yaw = FMath::FRand() * 360.f;

	if (Settings->AlignToNormal)
	{
		Inst.AlignToNormal(HitNormal, Settings->AlignMaxAngle);
	}

	// Apply the Z offset in local space
	if (FMath::Abs(Inst.ZOffset) > KINDA_SMALL_NUMBER)
	{
		Inst.Location = Inst.GetInstanceWorldTransform().TransformPosition(FVector(0, 0, Inst.ZOffset));
	}

	return true;
}