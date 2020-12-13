#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Metric/MetricBase.h"
#include "RelativeGapSize.generated.h"

UCLASS()
class AVOID_API ARelativeGapSize : public AMetricBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARelativeGapSize();

public:
	virtual bool Init(const FVector &start, const FVector &end) override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual TSharedPtr<FJsonObject> SerializeResults() override;

	virtual FString GetMetricName() override { return "RelativeGapSize"; };

	UPROPERTY(EditAnywhere)
	float DroneDiameter = 50;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	bool IsInitialised = false;
	float RGS = 0;
};
