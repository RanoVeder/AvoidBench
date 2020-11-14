#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AvoidMissionBase.generated.h"

UCLASS()
class AVOID_API AAvoidMission : public AInfo
{
	GENERATED_BODY()
public:
	UFUNCTION()
	FVector GetGlobalStartLocation();

	UFUNCTION()
	FVector GetGlobalFinishLocation();

	UPROPERTY(EditAnywhere)
	float MaxMissionTime = 300;

	UPROPERTY(EditAnywhere)
	float GoalRadius = 50;

	UPROPERTY(EditAnywhere)
	AVolume *MissionVolume;

protected:
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector StartLocation;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector EndLocation;
};
