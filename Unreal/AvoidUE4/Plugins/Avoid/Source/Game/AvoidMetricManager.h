#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Metric/MetricBase.h"
#include "AvoidMetricManager.generated.h"

UCLASS()
class AVOID_API UAvoidMetricManager : public UObject
{
	GENERATED_BODY()

public:
	UAvoidMetricManager();

	void Init(UWorld *world);

	void InitMetrics(const FVector &StartLocation, const FVector &EndLocation);
	void StartMetrics();
	void StopMetrics();
	void ResetMetrics();

	FString GetMetricResults();

private:

	UPROPERTY()
	UWorld *World;

	UPROPERTY()
	TArray<AMetricBase *> Metrics;
};
