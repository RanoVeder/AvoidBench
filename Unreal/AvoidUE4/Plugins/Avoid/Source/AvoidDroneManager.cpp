#include "AvoidDroneManager.h"
#include "SimHUD/SimHUD.h"
#include "Vehicles/Multirotor/SimModeWorldMultiRotor.h"
#include "Vehicles/Multirotor/MultirotorPawnSimApi.h"
#include "Vehicles/Multirotor/FlyingPawn.h"
#include "PawnSimApi.h"

#include "Kismet/GameplayStatics.h"

UAvoidDroneManager::UAvoidDroneManager(){

};

void UAvoidDroneManager::Init(UWorld *world)
{

	World = world;
	ASimHUD *hud = Cast<ASimHUD>(world->GetFirstPlayerController()->GetHUD());
	WorldApi = Cast<ASimModeWorldMultiRotor>(hud->GetSimModeBase());
	SimApi = (MultirotorPawnSimApi *)(WorldApi->getVehicleSimApi());
	ApiBase = SimApi->getVehicleApi();
	EnableApiControl();
};

AActor *UAvoidDroneManager::GetActor()
{

	TArray<AActor *> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AFlyingPawn::StaticClass(), FoundActors);

	if (FoundActors.Num() == 0)
	{
		return nullptr;
	}

	return FoundActors[0];
};

void UAvoidDroneManager::SetPosition(const FVector &Position, const float yaw)
{
	CancelTasks();
	MultirotorPawnSimApi::Pose pose = SimApi->getPose();
	NedTransform::Vector3r pos = WorldApi->getGlobalNedTransform().toGlobalNed(Position);
	pose.position = pos;
	pose.orientation = VectorMath::quaternionFromYaw(yaw);
	((PawnSimApi *)SimApi)->setPose(pose, true);
}

void UAvoidDroneManager::EnableControl()
{
	ControlEnabled = true;
	EnableApiControl();
}

void UAvoidDroneManager::DisableControl()
{
	ControlEnabled = false;
	if (ApiBase == nullptr)
		return;
	ApiBase->cancelLastTask();
}

FVector UAvoidDroneManager::ToAirsimNED(const FVector &position)
{
	if (WorldApi == nullptr)
		return FVector();
	auto NEDPosition = WorldApi->getGlobalNedTransform().toGlobalNed(position);
	return FVector(NEDPosition.x(), NEDPosition.y(), NEDPosition.z());
}

TFuture<void> UAvoidDroneManager::MoveToZ(float z)
{
	auto NEDPosition = WorldApi->getGlobalNedTransform().toGlobalNed(FVector(0, 0, z));
	TUniqueFunction<void()> Task = [=]() { BlockingMoveToZ(NEDPosition.z()); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::RotateToYaw(float yaw)
{
	count.Increment();
	TUniqueFunction<void()> Task = [=]() { BlockingRotateToYaw(yaw); count.Decrement(); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::moveByThrottle(float throttle1, float throttle2, float throttle3, float throttle4)
{
	count.Increment();
	TUniqueFunction<void()> Task = [=]() { BlockingmoveByThrottle(throttle1, throttle2, throttle3, throttle4); count.Decrement(); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveByRollPitchYawZ(float roll, float pitch, float yaw, float z)
{
	count.Increment();
	TUniqueFunction<void()> Task = [=]() { BlockingMoveByRollPitchYawZ(roll, pitch, yaw, z); count.Decrement(); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveByAngleRatesZ(float roll_rate, float pitch_rate, float yaw_rate, float z)
{
	count.Increment();
	TUniqueFunction<void()> Task = [=]() { BlockingMoveByAngleRatesZ(roll_rate, pitch_rate, yaw_rate, z); count.Decrement(); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveByVelocity(float vx, float vy, float vz, float yaw)
{
	TUniqueFunction<void()> Task = [=]() { BlockingMoveByVelocity(vx, vy, vz, yaw); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveToPosition(float x, float y, float z, float yaw)
{
	count.Increment();
	TUniqueFunction<void()> Task = [=]() { BlockingMoveToPosition(x, y, z, yaw); count.Decrement(); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

bool UAvoidDroneManager::HasCollisionOccured()
{
	if (SimApi == nullptr)
		return false;
	auto collisionInfo = SimApi->getCollisionInfo();
	return collisionInfo.has_collided;
}

//TODO: allow eigen vectors to be serialized by message pack.
avoid::rpc::DroneState UAvoidDroneManager::GetState()
{
	if (SimApi == nullptr)
		return avoid::rpc::DroneState{};
	auto state = SimApi->getGroundTruthKinematics();

	avoid::rpc::DroneState droneState;
	droneState.px = state->pose.position.x();
	droneState.py = state->pose.position.y();
	droneState.pz = state->pose.position.z();

	droneState.vx = state->twist.linear.x();
	droneState.vy = state->twist.linear.y();
	droneState.vz = state->twist.linear.z();

	droneState.ax = state->accelerations.linear.x();
	droneState.ay = state->accelerations.linear.y();
	droneState.az = state->accelerations.linear.z();

	msr::airlib::VectorMath::toEulerianAngle(state->pose.orientation, droneState.phi, droneState.theta, droneState.psi);

	droneState.p = state->twist.angular.x();
	droneState.q = state->twist.angular.y();
	droneState.r = state->twist.angular.z();

	droneState.pdot = state->accelerations.angular.x();
	droneState.qdot = state->accelerations.angular.y();
	droneState.rdot = state->accelerations.angular.z();

	return droneState;
}

void UAvoidDroneManager::CancelTasks()
{
	if (ApiBase == nullptr)
		return;
	ApiBase->cancelLastTask();
}

avoid::rpc::ImageResult UAvoidDroneManager::GetImage(const std::string &cameraName)
{
	if (SimApi == nullptr)
		return avoid::rpc::ImageResult();
	std::vector<ImageCaptureBase::ImageRequest> requests{ImageCaptureBase::ImageRequest(cameraName, ImageCaptureBase::ImageType::Scene, false, false)};

	auto result = SimApi->getImages(requests);
	if (result.size() == 0)
	{
		return avoid::rpc::ImageResult();
	}
	else
	{
		auto avoidImageResult = avoid::rpc::ImageResult();
		avoidImageResult.width = result[0].width;
		avoidImageResult.height = result[0].height;
		avoidImageResult.data = result[0].image_data_uint8;
		return avoidImageResult;
	}
}

std::vector<std::string> UAvoidDroneManager::GetAvailableCameras()
{
	std::vector<std::string> cameras;
	if (SimApi == nullptr)
		return cameras;

	AFlyingPawn *flyingPawn = Cast<AFlyingPawn>(SimApi->params_.pawn);
	if (flyingPawn == nullptr)
		return cameras;
	auto cameraMap = flyingPawn->getCameras().getMap();

	for (auto it = cameraMap.begin(); it != cameraMap.end(); ++it)
	{
		cameras.push_back(it->first);
	}

	return cameras;
}

void UAvoidDroneManager::SetCameraSettings(const std::string &cameraName, float fov)
{
	if (SimApi == nullptr)
		return;
	SimApi->setCameraFoV(cameraName, fov);
}

TFuture<void> UAvoidDroneManager::ExecuteAsync(EAsyncExecution executionMode, TUniqueFunction<void()> Task)
{
	if (ApiBase == nullptr || !ControlEnabled)
		return TFuture<void>();
	//EnableApiControl();

	TFuture<void> result = Async(executionMode, MoveTemp(Task));
	return result;
}

void UAvoidDroneManager::BlockingMoveToZ(float z)
{
	if (ApiBase == nullptr)
		return;
	ApiBase->moveToZ(z, 1.0f, 10, YawMode{true, 0}, 1.0f, false);
}

void UAvoidDroneManager::BlockingRotateToYaw(float yaw)
{
	if (ApiBase == nullptr)
		return;
	ApiBase->rotateToYaw(yaw, 99999, 5);
}

void UAvoidDroneManager::BlockingmoveByThrottle(float throttle1, float throttle2, float throttle3, float throttle4)
{
	if (ApiBase == nullptr)
		return;
	ApiBase->moveByMotorPWMs(throttle1, throttle2, throttle3, throttle4, 99999);
}

void UAvoidDroneManager::BlockingMoveByRollPitchYawZ(float roll, float pitch, float yaw, float z)
{
	if (ApiBase == nullptr)
		return;
	ApiBase->moveByRollPitchYawZ(roll, pitch, yaw, z, 99999);
}

void UAvoidDroneManager::BlockingMoveByAngleRatesZ(float roll_rate, float pitch_rate, float yaw_rate, float z)
{
	if (ApiBase == nullptr)
		return;
	ApiBase->moveByAngleRatesZ(roll_rate, pitch_rate, yaw_rate, z, 99999);
}

void UAvoidDroneManager::BlockingMoveByVelocity(float vx, float vy, float vz, float yaw)
{
	if (ApiBase == nullptr)
		return;

	float vxBody = cos(yaw) * vx - sin(yaw) * vy;
	float vyBody = sin(yaw) * vx + cos(yaw) * vy;
	float yawDegrees = yaw * 180.0f / PI;
	UE_LOG(LogAvoid, Warning, TEXT("vx, vy, vz, yaw: %f, %f, %f, %f"), vx, vy, vz, yawDegrees)
	ApiBase->moveByVelocity(vxBody, vyBody, vz, 99999, DrivetrainType::MaxDegreeOfFreedom, YawMode{false, yawDegrees});
}

void UAvoidDroneManager::BlockingMoveToPosition(float x, float y, float z, float yaw)
{
	if (ApiBase == nullptr)
		return;

	ApiBase->moveToPosition(x, y, z, 1, 99999, DrivetrainType::MaxDegreeOfFreedom, YawMode{false, yaw}, 0, 0);
}

void UAvoidDroneManager::EnableApiControl()
{
	if (ApiBase == nullptr)
		return;
	if (!ApiBase->isApiControlEnabled())
	{
		ApiBase->enableApiControl(true);
	}
	ApiBase->armDisarm(true);
}
