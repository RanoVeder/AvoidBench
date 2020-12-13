#include "LocationLogger.h"
#include "Util/OptimalPathCalculator.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

// Sets default values
ALocationLogger::ALocationLogger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Component"));
}

bool ALocationLogger::Init(const FVector &start, const FVector &end)
{
	AMetricBase::Init(start, end);

	PositionArray.Empty();
	
	AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GetWorld() == nullptr)
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

	UAvoidGameInstance* GameInstance = Cast<UAvoidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance == nullptr)
	{
		IsInitialised = false;
		return false;
	}

	ProceduralLocation = Cast<APoissonDisc>(GameInstance->LevelManager->GetActiveProceduralLocation());
	if (ProceduralLocation == nullptr)
	{
		IsInitialised = false;
		return false;
	}

	IsInitialised = true;
	IsRunning = false;
	Time = 0;

	return true;
}



void ALocationLogger::Start()
{
	IsRunning = true;
}

void ALocationLogger::Stop()
{
	IsRunning = false;
}

void ALocationLogger::Reset()
{
	Time = 0;
	PositionArray.Empty();
}

TSharedPtr<FJsonObject> ALocationLogger::SerializeResults()
{
	TSharedPtr<FJsonObject> out = MakeShareable(new FJsonObject);
	if (IsInitialised)
	{
		TArray< TSharedPtr<FJsonValue> > VectorArray;

		for (auto &vector : PositionArray)
		{
			TSharedPtr<FJsonObject> JsonVector = MakeShareable(new FJsonObject);
			JsonVector->SetNumberField("X", (int)vector.X);
			JsonVector->SetNumberField("Y", (int)vector.Y);
			JsonVector->SetNumberField("Z", (int)vector.Z);
			TSharedRef< FJsonValueObject > Val = MakeShareable(new FJsonValueObject(JsonVector));
			VectorArray.Add(Val);
		}
		out->SetArrayField("Positions", VectorArray);

		auto location = ProceduralLocation->GetObstacleLocations();
		TArray< TSharedPtr<FJsonValue> > ObstacleArray;

		for (auto& vector : location)
		{
			TSharedPtr<FJsonObject> JsonVector = MakeShareable(new FJsonObject);
			JsonVector->SetNumberField("X", (int)vector.X);
			JsonVector->SetNumberField("Y", (int)vector.Y);
			//this class is only created for the paper
			JsonVector->SetNumberField("Radius", 25);
			TSharedRef< FJsonValueObject > Val = MakeShareable(new FJsonValueObject(JsonVector));
			ObstacleArray.Add(Val);
		}

		out->SetArrayField("Obstacles", ObstacleArray);


		

	}
	else
	{
		out->SetStringField("Error", "Failed to initialise the Position History");
	}
	return out;
}

// Called when the game starts or when spawned
void ALocationLogger::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALocationLogger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Time += DeltaTime;

	if (IsRunning)
	{
		if (IsInitialised)
		{
			if (Time > 0.5)
			{
				PositionArray.Add(DroneActor->GetActorLocation());
				Time -= 0.5;
			}
		}
	}

}
