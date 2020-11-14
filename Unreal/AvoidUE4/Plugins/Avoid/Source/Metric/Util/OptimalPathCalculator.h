#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NavigationSystem.h"
#include "OptimalPathCalculator.generated.h"

/**
 * 
 */
UCLASS()
class AVOID_API UOptimalPathCalculator : public UObject
{
	GENERATED_BODY()
public:
	UOptimalPathCalculator();

	bool Init(UWorld *World, const FVector &start, const FVector &end);

	float GetPathLength();

private:
	bool GetOptimalPathLength(UWorld *World, const FVector &start, const FVector &end);

private:
	bool initialised = false;
	FPathFindingResult PathResult;
};
