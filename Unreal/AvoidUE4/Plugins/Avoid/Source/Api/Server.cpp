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

		try
		{
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
		}
		catch (...)
		{
			rpc::this_handler().respond_error("");
			return std::vector<std::string>();
		}
	});

	server->BindLoadWorld([this](const std::string &mapName) -> bool {
		
		try
		{
			UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(World->GetGameInstance());
			checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));

			bool success = false;
			TFunction<void()> Task = [&]() { GameInstance->LevelManager->LoadMap(UTF8_TO_TCHAR(mapName.c_str()), success); };
			FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
			FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);


			if (success == false)
			{
				rpc::this_handler().respond_error("Invalid map name");
				return false;
			}

			//You have to be carefull with updating variables from other threads. Compiler will optimize it into an infinite loop
			WorldReady.AtomicSet(false);
			while (WorldReady == false)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			WorldReady.AtomicSet(false);
			return true;
		}
		catch (...)
		{
			rpc::this_handler().respond_error("");
			return false;
		}
	});

	server->BindGetCurrentWorld([this]() -> std::string {
		try
		{
			UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(World->GetGameInstance());
			checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));
			FString result = GameInstance->LevelManager->GetCurrentMap();
			return std::string(TCHAR_TO_UTF8(*result));
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get current world.");
			return "";
		}
	});

	server->BindGetProceduralLocations([this]() -> std::vector<std::string> {
		try
		{
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
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get Failed to get procedural locations.");
			return std::vector<std::string>();
		}
	});

	server->BindSetProceduralLocation([this](const std::string &location, float radius, int seed) {
		try
		{
			UAvoidGameInstance *GameInstance = Cast<UAvoidGameInstance>(UGameplayStatics::GetGameInstance(World));
			checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));
			TFunction<void()> Task = [&]() { GameInstance->LevelManager->SetProceduralLocation(UTF8_TO_TCHAR(location.c_str()), radius, seed); };

			FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
			FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set procedural locations.");
			return;
		}
		return;
	});

	server->BindTakeoff([this]() {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->MoveToZ(-300);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to take off.");
			return;
		}
	});

	server->BindHover([this](float z) {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->MoveToZ(z);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set hover height.");
			return;
		}
	});

	//Potential bug (Calls to dronemanager might get queued on the same thread, waiting for other call to complete)
	server->BindSetControllerReferenceThrottle([this](float throttle1, float throttle2, float throttle3, float throttle4) {
		try
		{
			AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->moveByThrottle(throttle1, throttle2, throttle3, throttle4);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set controller reference.");
			return;
		}
	});

	server->BindSetControllerReferenceRate([this](float roll_rate, float pitch_rate, float yaw_rate, float z) {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->MoveByAngleRatesZ(roll_rate, pitch_rate, yaw_rate, z);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set controller reference.");
			return;
		}
	});

	server->BindSetControllerReferenceAttitude([this](float roll, float pitch, float yaw, float z) {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->MoveByRollPitchYawZ(roll, pitch, yaw, z);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set controller reference.");
			return;
		}
	});

	server->BindSetControllerReferenceVelocity([this](float vx, float vy, float vz, float yaw) {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->MoveByVelocity(vx, vy, vz, yaw);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set controller reference.");
			return;
		}
	});

	server->BindSetControllerReferencePosition([this](float x, float y, float z, float yaw) {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->MoveToPosition(x, y, z, yaw);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set controller reference.");
			return;
		}
	});

	server->BindCancelTasks([this]() {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->CancelTasks();
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to cancel tasks.");
			return;
		}
	});

	server->BindGetAvailableCameras([this]() -> std::vector<std::string> {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			return GameMode->DroneManager->GetAvailableCameras();
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get available cameras.");
			return std::vector<std::string>();
		}
	});

	server->BindSetCameraSettings([this](std::string cameraName, float fov) {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			GameMode->DroneManager->SetCameraSettings(cameraName, fov);
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to set camera settings.");
			return;
		}
	});

	server->BindGetImage([this](std::string cameraName) -> avoid::rpc::ImageResult {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));

			auto result = GameMode->DroneManager->GetImage(cameraName);
			return result;
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get image.");
			return avoid::rpc::ImageResult{};
		}
	});

	server->BindGetAvailableMissions([this]() -> std::vector<std::string> {
		try
		{
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
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get available missions.");
			return std::vector<std::string>();
		}
	});

	server->BindLoadMission([this](std::string missionName) -> bool {
		try
		{
			AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));

			//Function needs to run on gamethread
			bool success = false;
			TFunction<void()> Task = [&]() { GameMode->MissionManager->LoadMission(UTF8_TO_TCHAR(missionName.c_str()), success); };
			FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(Task), TStatId(), nullptr, ENamedThreads::GameThread);
			FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);

			if (success == false)
			{
				rpc::this_handler().respond_error("Mission does not exist");
				return false;
			}

			//Wait for mission to be in ready state. Check if provided mission is valid
			while (GameMode->MissionManager->GetMissionState() != EMissionState::Ready)
			{
				//wait
			}

			return true;
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to load mission");
			return false;
		}
	});

	server->BindLoadCustomMission([this](float, float) {

	});

	server->BindGetMissionState([this]() -> int {
		try
		{
			AAvoidGameMode *GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			return (int)GameMode->MissionManager->GetMissionState();
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get mission state");
			return false;
		}
	});

	server->BindGetMissionInfo([this]() -> avoid::rpc::MissionInfo {
		try
		{
			AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
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
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get mission info");
			return avoid::rpc::MissionInfo{};
		}
		
	});

	server->BindStartMission([this] {

		try
		{
			AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			if (GameMode->MissionManager->StartMission())
			{
				//wait for mission to start before returning (Otherwise the client can send control commands before control is enabled)
				while (GameMode->MissionManager->GetMissionState() != EMissionState::Running)
				{
					//wait
				}
			}
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to start mission");
		}
		
	});

	server->BindStartFreeFlight([this]() {

	});

	server->BindGetDroneState([this]() -> avoid::rpc::DroneState {
		try
		{
			AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			return GameMode->DroneManager->GetState();
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get drone state");
			return avoid::rpc::DroneState{};
		}
		
	});

	server->BindGetMetricResults([this]() -> std::string {
		try
		{
			AAvoidGameMode* GameMode = Cast<AAvoidGameMode>(UGameplayStatics::GetGameMode(World));
			checkf(GameMode != nullptr, TEXT("Did you enable the right gamemode?"));
			FString Results = GameMode->MetricManager->GetMetricResults();
			return std::string(TCHAR_TO_UTF8(*Results));
		}
		catch (...)
		{
			rpc::this_handler().respond_error("Failed to get metric results");
			return "";
		}
		
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
