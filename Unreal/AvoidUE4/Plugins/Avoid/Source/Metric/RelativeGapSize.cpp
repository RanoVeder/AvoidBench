#include "RelativeGapSize.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Procedural/PoissonDisc.h"

#include "DrawDebugHelpers.h"

// Sets default values
ARelativeGapSize::ARelativeGapSize()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Component"));
}

bool ARelativeGapSize::Init(const FVector &start, const FVector &end)
{
	RGS = 0;

	if (GetWorld() == nullptr)
	{
		IsInitialised = false;
		return false;
	}

	UAvoidGameInstance* GameInstance = Cast<UAvoidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance == nullptr)
	{
		IsInitialised = false;
		return false;
	}


	APoissonDisc* ProceduralLocation = Cast<APoissonDisc>(GameInstance->LevelManager->GetActiveProceduralLocation());
	if (ProceduralLocation == nullptr)
	{
		IsInitialised = false;
		return false;
	}

	auto Radius = ProceduralLocation->Radius;

	float AverageWidth = 0;

	for (int i = 0; i < ProceduralLocation->FoliageTypes.Num(); i++)
	{
		auto BoundingBox = ProceduralLocation->FoliageTypes[i]->Mesh->GetBoundingBox();
		FVector Size = BoundingBox.GetSize();
		AverageWidth += ((Size.X + Size.Y) / 2.0);
	}
	AverageWidth /= ProceduralLocation->FoliageTypes.Num();

	RGS = (Radius - AverageWidth) / DroneDiameter;
	IsInitialised = true;

	return true;
}

void ARelativeGapSize::Start()
{
}

void ARelativeGapSize::Stop()
{
}

void ARelativeGapSize::Reset()
{
	RGS = 0;
}

TSharedPtr<FJsonObject> ARelativeGapSize::SerializeResults()
{
	TSharedPtr<FJsonObject> out = MakeShareable(new FJsonObject);
	if (IsInitialised)
	{
		out->SetNumberField("RelativeGapSize", RGS);
		//Convert to metric
		out->SetNumberField("DroneDiameter", DroneDiameter/100.0);
	}
	else
	{
		out->SetStringField("Error", "Failed to initialise the APR Metric");
	}
	return out;
}

// Called when the game starts or when spawned
void ARelativeGapSize::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARelativeGapSize::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
