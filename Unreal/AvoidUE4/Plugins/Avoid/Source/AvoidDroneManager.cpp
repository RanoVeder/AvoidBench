#include "AvoidDroneManager.h"
#include "SimHUD/SimHUD.h"
#include "Vehicles/Multirotor/SimModeWorldMultiRotor.h"
#include "Vehicles/Multirotor/MultirotorPawnSimApi.h"
#include "Vehicles/Multirotor/FlyingPawn.h"
#include "PawnSimApi.h"

#include "Kismet/GameplayStatics.h"

UAvoidDroneManager::UAvoidDroneManager(){

};

UAvoidDroneManager::~UAvoidDroneManager() {
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

bool UAvoidDroneManager::AirsimInstancesValid()
{

	if (World->GetFirstPlayerController() == nullptr)
	{
		return false;
	}

	ASimHUD* hud = Cast<ASimHUD>(World->GetFirstPlayerController()->GetHUD());
	if (hud == nullptr)
	{
		return false;
	}

	WorldApi = Cast<ASimModeWorldMultiRotor>(hud->GetSimModeBase());
	if (WorldApi == nullptr)
	{
		return false;
	}

	SimApi = (MultirotorPawnSimApi*)(WorldApi->getVehicleSimApi());
	if (SimApi == nullptr)
	{
		return false;
	}

	ApiBase = SimApi->getVehicleApi();
	if (ApiBase == nullptr)
	{
		return false;
	}
	return true;
}

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

FVector UAvoidDroneManager::ToAirsimNED(const FVector& position)
{
	if (AirsimInstancesValid() == false)
		throw std::runtime_error("Airsim instance nullptr encounterd.");
	
	auto NEDPosition = WorldApi->getGlobalNedTransform().toGlobalNed(position);
	return FVector(NEDPosition.x(), NEDPosition.y(), NEDPosition.z());
}

TFuture<void> UAvoidDroneManager::MoveToZ(float z)
{
	if (AirsimInstancesValid() == false)
		throw std::runtime_error("Airsim instance nullptr encounterd.");

	auto NEDPosition = WorldApi->getGlobalNedTransform().toGlobalNed(FVector(0, 0, z));
	TUniqueFunction<void()> Task = [=]() { BlockingMoveToZ(NEDPosition.z()); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::RotateToYaw(float yaw)
{
	TUniqueFunction<void()> Task = [=]() { BlockingRotateToYaw(yaw);};
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::moveByThrottle(float throttle1, float throttle2, float throttle3, float throttle4)
{
	TUniqueFunction<void()> Task = [=]() { BlockingmoveByThrottle(throttle1, throttle2, throttle3, throttle4); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveByRollPitchYawZ(float roll, float pitch, float yaw, float z)
{
	TUniqueFunction<void()> Task = [=]() { BlockingMoveByRollPitchYawZ(roll, pitch, yaw, z); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveByAngleRatesZ(float roll_rate, float pitch_rate, float yaw_rate, float z)
{
	TUniqueFunction<void()> Task = [=]() { BlockingMoveByAngleRatesZ(roll_rate, pitch_rate, yaw_rate, z); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveByVelocity(float vx, float vy, float vz, float yaw)
{
	TUniqueFunction<void()> Task = [=]() { BlockingMoveByVelocity(vx, vy, vz, yaw); };
	return ExecuteAsync(EAsyncExecution::ThreadPool, MoveTemp(Task));
}

TFuture<void> UAvoidDroneManager::MoveToPosition(float x, float y, float z, float yaw)
{
	TUniqueFunction<void()> Task = [=]() { BlockingMoveToPosition(x, y, z, yaw); };
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

avoid::rpc::ImageResult UAvoidDroneManager::GetImage(std::string cameraName)
{
	if (AirsimInstancesValid() == false)
		throw std::runtime_error("aa");

	std::vector<ImageCaptureBase::ImageRequest> requests{ImageCaptureBase::ImageRequest(cameraName, ImageCaptureBase::ImageType::Scene, false, false)};

	auto result = SimApi->getImages(requests);
	if (result.size() == 0)
	{
		return avoid::rpc::ImageResult();
	}
	else
	{
		//Sometimes airsim returns empty results because it is unable to obtain a world handle from the camera. I currently have no clue what causes this. Make sure to check if image is empty
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
	if (AirsimInstancesValid() == false || !ControlEnabled)
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
