#include "AvoidMissionManager.h"
#include "AvoidLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Vehicles/Multirotor/FlyingPawn.h"

//temp
#include "Metric/OptimalityFactor.h"

#include "AvoidDroneManager.h"

UAvoidMissionManager::UAvoidMissionManager()
{
	CurrentMission = nullptr;
	CurrentState = EMissionState::NotLoaded;
	LastViewTarget = nullptr;
}

void UAvoidMissionManager::Init(UWorld *world, UAvoidDroneManager *droneManager, UAvoidMetricManager *metricManager)
{
	checkf(world != nullptr, TEXT("World is nullptr."));
	World = world;
	DroneManager = droneManager;
	MetricManager = metricManager;
}

void UAvoidMissionManager::Tick(float DeltaSeconds)
{

	switch (CurrentState)
	{

	case EMissionState::NotLoaded:
	{

		if (LevelOverviewFocus == false)
		{
			APlayerController *PlayerController = UGameplayStatics::GetPlayerController(World, 0);
			AActor *actor = GetLevelOverviewCamera();
			if (actor != nullptr)
			{
				LastViewTarget = PlayerController->GetViewTarget();
				FViewTargetTransitionParams params;
				PlayerController->ClientSetViewTarget(actor, params);
			}
			LevelOverviewFocus = true;
		}
		if (TaskGotLoaded)
		{
			CurrentState = EMissionState::Initialise;
			TaskGotLoaded = false;
		}
	}
	break;

	case EMissionState::Initialise:
		if (!IsInitializing)
		{
			if (CurrentMission == nullptr)
			{
				CurrentState = EMissionState::NotLoaded;
			}
			else
			{
				IsInitializing = true;

				MissionTimer = 0;

				FVector StartLocation = CurrentMission->GetGlobalStartLocation();
				FVector EndLocation = CurrentMission->GetGlobalFinishLocation();

				float yaw = atan2(EndLocation.Y - StartLocation.Y, EndLocation.X - StartLocation.X);
				DroneManager->SetPosition(StartLocation, yaw);

				DroneManager->EnableControl();
				//Initializing = DroneManager->BlockingMoveToZ(StartLocation.Z);
				//DroneManager->MoveToZ(StartLocation.Z + 10);
				DroneManager->MoveByVelocity(0, 0, 0, yaw);
				DroneManager->DisableControl();

				if (LastViewTarget != nullptr)
				{
					APlayerController *PlayerController = UGameplayStatics::GetPlayerController(World, 0);
					FViewTargetTransitionParams params;
					PlayerController->ClientSetViewTarget(LastViewTarget, params);
					LevelOverviewFocus = false;
				}

				MetricManager->InitMetrics(StartLocation, EndLocation);
				MetricManager->ResetMetrics();
			}
		}
		CurrentState = EMissionState::Ready;

		//if (Initializing.IsReady())
		//{
		//	UE_LOG(LogAvoid, Warning, TEXT("Mission Ready"))
		//	CurrentState = EMissionState::Ready;
		//}

		break;
	case EMissionState::Ready:
		if (MissionShouldStart)
		{
			CurrentState = EMissionState::Running;
			DroneManager->EnableControl();
			MissionShouldStart = false;

			MetricManager->StartMetrics();
		}
		break;

	case EMissionState::Running:
		MissionTimer += DeltaSeconds;

		if (DroneManager->HasCollisionOccured())
		{
			CurrentState = EMissionState::Ended;
			UE_LOG(LogAvoid, Warning, TEXT("Collision Has occured"))
			DroneManager->DisableControl();

			MetricManager->StopMetrics();
		}

		if (HasReachedFinish())
		{
			CurrentState = EMissionState::Ended;
			UE_LOG(LogAvoid, Warning, TEXT("%f"), MissionTimer)
			DroneManager->DisableControl();

			MetricManager->StopMetrics();
		}

		if (MissionTimer >= CurrentMission->MaxMissionTime)
		{
			CurrentState = EMissionState::Ended;
			UE_LOG(LogAvoid, Warning, TEXT("Exceeded maximum mission time"), MissionTimer)
			DroneManager->DisableControl();

			MetricManager->StopMetrics();
		}

		break;
	case EMissionState::Ended:
		break;
	}
	return;
}

EMissionState UAvoidMissionManager::GetMissionState()
{
	return CurrentState;
}

bool UAvoidMissionManager::IsComplete()
{
	if (CurrentState == EMissionState::Ended)
	{
		return true;
	}
	return false;
}

void UAvoidMissionManager::GetAvailableMissions(TArray<FString> &missions)
{
	TArray<AActor *> AllActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAvoidMission::StaticClass(), AllActors);

	for (auto Actor : AllActors)
	{
		missions.Add(Actor->GetHumanReadableName());
	}
}

FString UAvoidMissionManager::GetCurrentMissionName()
{
	if (CurrentMission == nullptr)
		return FString("");
	return CurrentMission->GetHumanReadableName();
}

AAvoidMission *UAvoidMissionManager::GetCurrentMission()
{
	return CurrentMission;
}

float UAvoidMissionManager::GetMissionTime()
{
	return MissionTimer;
}

bool UAvoidMissionManager::LoadMission(const FString &MissionName)
{
	//ensure only tasks can be loaded while in not running stastes..
	UE_LOG(LogAvoid, Warning, TEXT("Loading mission: %s"), *MissionName)
	bool FoundActor = false;
	TArray<AActor *> AllActors;

	UGameplayStatics::GetAllActorsOfClass(World, AAvoidMission::StaticClass(), AllActors);

	for (auto Actor : AllActors)
	{
		UE_LOG(LogAvoid, Warning, TEXT("%s"), *Actor->GetHumanReadableName())
		if (Actor->GetHumanReadableName() == MissionName)
		{
			CurrentMission = Cast<AAvoidMission>(Actor);
			FoundActor = true;
			TaskGotLoaded = true;
			break;
		};
	}

	return FoundActor;
}

bool UAvoidMissionManager::StartMission()
{
	if (CurrentState == EMissionState::Ready)
	{
		MissionShouldStart = true;
		return true;
	}
	return false;
}

AActor *UAvoidMissionManager::GetLevelOverviewCamera()
{
	TArray<AActor *> AllActors;
	UGameplayStatics::GetAllActorsWithTag(World, TEXT("OverviewCamera"), AllActors);
	if (AllActors.Num() == 0)
		return nullptr;
	return AllActors[0];
}

bool UAvoidMissionManager::HasReachedFinish()
{
	//TODO: move to dronemanager class + dont do this every tick.
	TArray<AActor *> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AFlyingPawn::StaticClass(), AllActors);
	if (AllActors.Num() == 0)
		return false;

	FVector dronePosition = AllActors[0]->GetActorLocation();
	FVector endLocation = CurrentMission->GetGlobalFinishLocation();

	FVector2D difference(dronePosition.X - endLocation.X, dronePosition.Y - endLocation.Y);

	if (difference.Size() < CurrentMission->GoalRadius)
	{
		return true;
	}
	else
	{
		return false;
	}
}
