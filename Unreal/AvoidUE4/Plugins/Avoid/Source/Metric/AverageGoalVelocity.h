#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Metric/MetricBase.h"
#include "AverageGoalVelocity.generated.h"

UCLASS()
class AVOID_API AAverageGoalVelocity : public AMetricBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAverageGoalVelocity();

public:
	virtual bool Init(const FVector &start, const FVector &end) override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual TSharedPtr<FJsonObject> SerializeResults() override;

	virtual FString GetMetricName() override { return "AverageGoalVelocity"; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	bool IsInitialised = false;
	bool Running = false;

	double MissionTime = 0;
	double ReferenceDistance = 1;
};
