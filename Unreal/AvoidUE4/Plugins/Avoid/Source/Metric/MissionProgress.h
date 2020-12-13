#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Metric/MetricBase.h"
#include "MissionProgress.generated.h"

UCLASS()
class AVOID_API AMissionProgress : public AMetricBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMissionProgress();

public:
	virtual bool Init(const FVector &start, const FVector &end) override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual TSharedPtr<FJsonObject> SerializeResults() override;

	virtual FString GetMetricName() override { return "Mission Progress"; };

	float GetMissionProgress();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY()
	AActor* DroneActor;

	bool IsInitialised = false;
	bool Running = false;


	float GoalRadius = 0;
	FVector CurrentPosition;
};
