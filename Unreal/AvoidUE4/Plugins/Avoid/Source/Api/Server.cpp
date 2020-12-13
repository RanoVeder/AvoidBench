#include "Server.h"

#include <compiler/disable-ue4-macros.h>
#include <avoid/RPCServer.h>
#include <compiler/enable-ue4-macros.h>
#include "Async.h"
#include "Game/AvoidGameInstance.h"
#include "Game/AvoidGameMode.h"
#include "Kismet/GameplayStatics.h"
#include <chrono>

//temporary remove
#include <chrono>
#include <thread>

#include <iostream>

Server::Server() : server(nullptr), World(nullptr)
{
}

Server::~Server()
{
	if (server != nullptr)
		delete server;
}

void Server::BindCallbacks()
{

	server->BindGetAvailableWorlds([this]() -> std::vector<std::string> {
		//UE_LOG(LogAvoid, Warning, TEXT("World: %p"), World());

		UE_LOG(LogTemp, Warning, TEXT("I just started running"));

		UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(World->GetGameInstance());
		checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));
		TArray<FString> results = GameInstance->LevelManager->GetAvailableMaps();
		std::vector<std::string> returnResults;
		returnResults.reserve(results.Num());
		for (auto &map : results)
		{
			std::string tmp = std::string(TCHAR_TO_UTF8(*map));
			returnResults.emplace_back(tmp);
		}
		return returnResults;
	});

	server->BindLoadWorld([this](const std::string &mapName) -> bool {
		UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(World->GetGameInstance());
		checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));

		TFunction<void()> Task = [&]() { GameInstance->LevelManager->LoadMap(UTF8_TO_TCHAR(mapName.c_str())); };

		FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);

		//You have to be carefull with updating variables from other threads. Compiler will optimize it into an infinite loop

		WorldReady.AtomicSet(false);
		while (WorldReady == false)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		WorldReady.AtomicSet(false);
		return true;
	});

	server->BindGetCurrentWorld([this]() -> std::string {
		UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(World->GetGameInstance());
		checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));
		FString result = GameInstance->LevelManager->GetCurrentMap();
		return std::string(TCHAR_TO_UTF8(*result));
	});

	server->BindGetProceduralLocations([this]() -> std::vector<std::string> {
		UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(World->GetGameInstance());
		checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));

		TArray<FString> locations;
		TFunction<void()> Task = [&]() { GameInstance->LevelManager->GetProceduralLocations(locations); };

		FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);

		std::vector<std::string> returnResults;
		returnResults.reserve(locations.Num());
		for (auto &map : locations)
		{
			std::string tmp = std::string(TCHAR_TO_UTF8(*map));
			returnResults.emplace_back(tmp);
		}
		return returnResults;
	});

	server->BindSetProceduralLocation([this](const std::string &location, float radius, int seed) {
		UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(UGameplayStatics::GetGameInstance(World));
		checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));
		TFunction<void()> Task = [&]() { GameInstance->LevelManager->SetProceduralLocation(UTF8_TO_TCHAR(location.c_str()), radius, seed); };

		FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);
		return;
	});

	server->BindTakeoff([this]() {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->MoveToZ(-300);
	});

	server->BindHover([this](float z) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->MoveToZ(z);
	});

	//Potential bug (Calls to dronemanager might get queued on the same thread, waiting for other call to complete)
	server->BindSetControllerReferenceThrottle([this](float throttle1, float throttle2, float throttle3, float throttle4) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->moveByThrottle(throttle1, throttle2, throttle3, throttle4);
	});

	server->BindSetControllerReferenceRate([this](float roll_rate, float pitch_rate, float yaw_rate, float z) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->MoveByAngleRatesZ(roll_rate, pitch_rate, yaw_rate, z);
	});

	server->BindSetControllerReferenceAttitude([this](float roll, float pitch, float yaw, float z) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->MoveByRollPitchYawZ(roll, pitch, yaw, z);
	});

	server->BindSetControllerReferenceVelocity([this](float vx, float vy, float vz, float yaw) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->MoveByVelocity(vx, vy, vz, yaw);
	});

	server->BindSetControllerReferencePosition([this](float x, float y, float z, float yaw) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->MoveToPosition(x, y, z, yaw);
	});

	server->BindCancelTasks([this]() {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->CancelTasks();
	});

	server->BindGetAvailableCameras([this]() -> std::vector<std::string> {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		return GameMode->DroneManager->GetAvailableCameras();
	});

	server->BindSetCameraSettings([this](std::string cameraName, float fov) {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		GameMode->DroneManager->SetCameraSettings(cameraName, fov);
	});

	server->BindGetImage([this](std::string cameraName) -> avoid::rpc::ImageResult {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));

		auto result = GameMode->DroneManager->GetImage(cameraName);
		return result;
	});

	server->BindGetAvailableMissions([this]() -> std::vector<std::string> {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));

		TArray<FString> missions;

		TFunction<void()> Task = [&]() { GameMode->MissionManager->GetAvailableMissions(missions); };
		FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);

		std::vector<std::string> returnResults;
		returnResults.reserve(missions.Num());

		for (auto &mission : missions)
		{
			std::string tmp = std::string(TCHAR_TO_UTF8(*mission));
			returnResults.emplace_back(tmp);
		}
		return returnResults;
	});

	server->BindLoadMission([this](std::string missionName) -> bool {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));

		//Function needs to run on gamethread
		TFunction<void()> Task = [&]() { GameMode->MissionManager->LoadMission(UTF8_TO_TCHAR(missionName.c_str())); };
		FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);

		//Wait for mission to be in ready state
		while (GameMode->MissionManager->GetMissionState() != EMissionState::Ready)
		{
			//wait
		}

		return true;
	});

	server->BindLoadCustomMission([this](float, float) {

	});

	server->BindGetMissionState([this]() -> int {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		return (int)GameMode->MissionManager->GetMissionState();
	});

	server->BindGetMissionInfo([this]() -> avoid::rpc::MissionInfo {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		auto mission = GameMode->MissionManager->GetCurrentMission();
		double time = GameMode->MissionManager->GetMissionTime();
		FVector startPosition = GameMode->DroneManager->ToAirsimNED(mission->GetGlobalStartLocation());
		FVector endPosition = GameMode->DroneManager->ToAirsimNED(mission->GetGlobalFinishLocation());

		avoid::rpc::MissionInfo info;
		info.startx = startPosition.X;
		info.starty = startPosition.Y;
		info.startz = startPosition.Z;

		info.endx = endPosition.X;
		info.endy = endPosition.Y;
		info.endz = endPosition.Z;

		info.goalHeading = 0; // atan2(endPosition.Y - startPosition.Y, endPosition.X - startPosition.X);
		info.time = (double)time;

		return info;
	});

	server->BindStartMission([this] {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		if (GameMode->MissionManager->StartMission())
		{
			//wait for mission to start before returning (Otherwise the client can send control commands before control is enabled)
			while (GameMode->MissionManager->GetMissionState() != EMissionState::Running)
			{
				//wait
			}
		}
	});

	server->BindStartFreeFlight([this]() {

	});

	server->BindGetDroneState([this]() -> avoid::rpc::DroneState {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		return GameMode->DroneManager->GetState();
	});

	server->BindGetMetricResults([this]() -> std::string {
		AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
		checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
		FString Results = GameMode->MetricManager->GetMetricResults();
		return std::string(TCHAR_TO_UTF8(*Results));
	});
}

void Server::Start(uint16_t port)
{
	server = new avoid::rpc::RPCServer(port);
	BindCallbacks();
	server->Start(1);
}

void Server::Stop()
{
	server->Stop();
}

void Server::SetWorld(UWorld *world)
{
	World = world;
}

void Server::WorldLoadingFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("WorldReady before loadcomplete: %d"), (bool)WorldReady);
	WorldReady.AtomicSet(true);
	UE_LOG(LogTemp, Warning, TEXT("WorldReady after loadcomplete: %d"), (bool)WorldReady);
}
