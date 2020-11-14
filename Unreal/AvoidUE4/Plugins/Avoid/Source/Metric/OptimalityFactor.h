#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Metric/MetricBase.h"
#include "OptimalityFactor.generated.h"

UCLASS()
class AVOID_API AOptimalityFactor : public AMetricBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOptimalityFactor();

public:
	// Called every frame
	float GetOptimalityFactor();

	virtual bool Init(const FVector &start, const FVector &end) override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual TSharedPtr<FJsonObject> SerializeResults() override;

	virtual FString GetMetricName() override { return "OptimalityFactor"; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	bool IsInitialised = false;
	bool running = false;
	AActor *DroneActor;

	FVector PreviousLocation;

	float ReferenceDistance = 0;
	float TravelledDistance = 0;
};
