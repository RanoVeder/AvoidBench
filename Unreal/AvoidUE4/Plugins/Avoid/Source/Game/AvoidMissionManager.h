#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Mission/AvoidMissionBase.h"
#include "Async/Future.h"
#include "HAL/ThreadSafeBool.h"
#include "Templates/Atomic.h"
#include "AvoidMetricManager.h"
#include "AvoidMissionManager.generated.h"

UENUM()
enum class EMissionState
{
	NotLoaded,
	Initialise,
	Ready,
	Running,
	Ended
};

class UAvoidDroneManager;

 UCLASS()
class AVOID_API UAvoidMissionManager : public UObject
{
	GENERATED_BODY()

public:
	UAvoidMissionManager();

	void Init(UWorld *world, UAvoidDroneManager *droneManager, UAvoidMetricManager *metricManager);
	void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	EMissionState GetMissionState();

	UFUNCTION(BlueprintCallable)
	bool IsComplete();

	UFUNCTION(BlueprintCallable)
	void GetAvailableMissions(TArray<FString> &missions);

	UFUNCTION(BlueprintCallable)
	FString GetCurrentMissionName();

	UFUNCTION(BlueprintCallable)
	AAvoidMission *GetCurrentMission();

	UFUNCTION(BlueprintCallable)
	float GetMissionTime();

	UFUNCTION(BlueprintCallable, Exec)
	bool LoadMission(const FString &MissionName);

	UFUNCTION(BlueprintCallable, Exec)
	bool StartMission();

	UFUNCTION(BlueprintCallable, Exec)
	AActor *GetLevelOverviewCamera();

	bool HasReachedFinish();

private:
	float MissionTimer;

	TAtomic<EMissionState> CurrentState;
	AAvoidMission *CurrentMission;
	AActor *LastViewTarget;

	UWorld *World;
	UAvoidDroneManager *DroneManager;
	UAvoidMetricManager *MetricManager;

	FThreadSafeBool LevelOverviewFocus = false;
	FThreadSafeBool TaskGotLoaded = false;
	FThreadSafeBool IsInitializing = false;
	FThreadSafeBool MissionShouldStart = false;
	TFuture<void> Initializing;

	bool bIsInitializingDone = false;
};
