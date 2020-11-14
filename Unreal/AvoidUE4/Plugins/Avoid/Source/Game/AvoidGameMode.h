#pragma once

#include "CoreMinimal.h"
#include "AirSimGameMode.h"
#include "AvoidGameInstance.h"
#include "AvoidDroneManager.h"
#include "AvoidMissionManager.h"
#include "AvoidMetricManager.h"
#include "Metric/Util/OptimalPathCalculator.h"

#include "AvoidGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AVOID_API AAvoidGameMode : public AAirSimGameMode
{
    GENERATED_BODY()
public:
    AAvoidGameMode(const FObjectInitializer &ObjectInitializer);

    UPROPERTY()
    UAvoidDroneManager *DroneManager;

    UPROPERTY()
    UAvoidMissionManager *MissionManager;

    UPROPERTY()
    UAvoidMetricManager *MetricManager;

    UPROPERTY()
    UOptimalPathCalculator *nav;

protected:
    //Overrides
    void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) override;
    void StartPlay() override;
    void Tick(float DeltaSeconds) override;
    void StartToLeaveMap() override;

private:
    UFUNCTION(Exec)
    void MoveToZ(float z);

    UFUNCTION(Exec)
    void LoadTask(const FString &TaskName);

    UFUNCTION(Exec)
    void StartMission();

    UFUNCTION(Exec)
    void SetPosition(float X, float Y, float Z, float yaw);

    UFUNCTION(Exec)
    void Metric();

    UPROPERTY()
    UAvoidGameInstance *GameInstance;

    TFuture<void> result;

    float time = 0;
};
