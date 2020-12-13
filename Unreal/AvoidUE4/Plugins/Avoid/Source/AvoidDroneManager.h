#pragma once

#include <compiler/disable-ue4-macros.h>
#include <avoid/types/DroneState.h>
#include <avoid/types/ImageResult.h>
#include <compiler/enable-ue4-macros.h>

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/ThreadSafeCounter.h"
#include "AvoidDroneManager.generated.h"

namespace msr
{
	namespace airlib
	{
		class MultirotorApiBase;
	}
} // namespace msr

UCLASS()
class AVOID_API UAvoidDroneManager : public UObject
{
	GENERATED_BODY()

public:
	UAvoidDroneManager();

	void Init(UWorld *world);
	AActor *GetActor();
	void SetPosition(const FVector &Position, const float yaw);

	void EnableControl();
	void DisableControl();

	FVector ToAirsimNED(const FVector &position);

	TFuture<void> MoveToZ(float z);
	TFuture<void> RotateToYaw(float yaw);
	TFuture<void> moveByThrottle(float throttle1, float throttle2, float throttle3, float throttle4);
	TFuture<void> MoveByRollPitchYawZ(float roll, float pitch, float yaw, float z);
	TFuture<void> MoveByAngleRatesZ(float roll_rate, float pitch_rate, float yaw_rate, float z);
	TFuture<void> MoveByVelocity(float vx, float vy, float vz, float yaw);
	TFuture<void> MoveToPosition(float x, float y, float z, float yaw);

	//TODO: Don't use avoidlib structs in UE4 code, should only be used in server.cpp to prevent unwanted dependencies
	avoid::rpc::DroneState GetState();

	avoid::rpc::ImageResult GetImage(std::string cameraName);
	std::vector<std::string> GetAvailableCameras();
	void SetCameraSettings(const std::string &cameraName, float fov);

	bool HasCollisionOccured();

	void CancelTasks();

	void BlockingmoveByThrottle(float throttle1, float throttle2, float throttle3, float throttle4);
	void BlockingMoveByRollPitchYawZ(float roll, float pitch, float yaw, float z);
	void BlockingMoveByAngleRatesZ(float roll_rate, float pitch_rate, float yaw_rate, float z);

	UFUNCTION(BlueprintCallable)
	void BlockingMoveByVelocity(float vx, float vy, float vz, float yaw);
	void BlockingMoveToPosition(float x, float y, float z, float yaw);

	void BlockingMoveToZ(float z);
	void BlockingRotateToYaw(float z);

private:
	TFuture<void> ExecuteAsync(EAsyncExecution executionMode, TUniqueFunction<void()> Task);
	void EnableApiControl();


private:
	bool ControlEnabled = false;

	UPROPERTY()
	UWorld *World;

	msr::airlib::MultirotorApiBase *ApiBase;
	class MultirotorPawnSimApi *SimApi;
	class ASimModeWorldMultiRotor *WorldApi;
};
