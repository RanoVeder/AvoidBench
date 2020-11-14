#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Metric/MetricBase.h"
#include "Traversability.generated.h"

UCLASS()
class AVOID_API ATraversability : public AMetricBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATraversability();

public:
	virtual bool Init(const FVector &start, const FVector &end) override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual TSharedPtr<FJsonObject> SerializeResults() override;

	virtual FString GetMetricName() override { return "Traversability"; };

	UPROPERTY(EditAnywhere)
	AVolume *MeasureArea;

	UPROPERTY(EditAnywhere)
	float GridSize = 100;

	UPROPERTY(EditAnywhere)
	float MaxRayLength = 1000;

	UPROPERTY(EditAnywhere)
	float SamplesPerPosition = 16;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void SamplePosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	bool running = false;
	float Traversability = 0;
};
