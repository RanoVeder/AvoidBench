#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Metric/MetricBase.h"
#include "Procedural/PoissonDisc.h"
#include "LocationLogger.generated.h"

UCLASS()
class AVOID_API ALocationLogger : public AMetricBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALocationLogger();

public:
	virtual bool Init(const FVector &start, const FVector &end) override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual TSharedPtr<FJsonObject> SerializeResults() override;

	virtual FString GetMetricName() override { return "LocationLogger"; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	bool IsInitialised = false;
	bool IsRunning = false;
	double Time = 0;
	
	UPROPERTY()
	AActor* DroneActor;

	UPROPERTY()
	APoissonDisc* ProceduralLocation;
	
	TArray<FVector> PositionArray;
};
