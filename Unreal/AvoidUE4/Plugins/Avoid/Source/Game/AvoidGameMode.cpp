#include "AvoidGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "SimHUD/SimHUD.h"
#include "Vehicles/Multirotor/SimModeWorldMultiRotor.h"
#include "Vehicles/Multirotor/MultirotorPawnSimApi.h"

AAvoidGameMode::AAvoidGameMode(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{

	//Create level manager

	DroneManager = CreateDefaultSubobject<UAvoidDroneManager>(TEXT("AvoidDroneManager"));
	MissionManager = CreateDefaultSubobject<UAvoidMissionManager>(TEXT("AvoidMissionManager"));
	MetricManager = CreateDefaultSubobject<UAvoidMetricManager>(TEXT("AvoidMetricManager"));
	nav = CreateDefaultSubobject<UOptimalPathCalculator>(TEXT("OptimalPathCalculator"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AAvoidGameMode::InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GameInstance = GetWorld()->GetGameInstance<UAvoidGameInstance>();
	checkf(GameInstance != nullptr, TEXT("Set UAvoidGameInstance as default in project settings."));
}

void AAvoidGameMode::StartPlay()
{
	Super::StartPlay();
	DroneManager->Init(GetWorld());
	MetricManager->Init(GetWorld());
	MissionManager->Init(GetWorld(), DroneManager, MetricManager);
}

void AAvoidGameMode::StartToLeaveMap()
{
	DroneManager->CancelTasks();
	Super::StartToLeaveMap();
}

void AAvoidGameMode::MoveToZ(float z)
{

	result = DroneManager->MoveToZ(z);
}

void AAvoidGameMode::SetPosition(float X, float Y, float Z, float yaw)
{
	DroneManager->SetPosition(FVector(X, Y, Z), yaw);
}

void AAvoidGameMode::LoadTask(const FString &TaskName)
{
	MissionManager->LoadMission(TaskName);
}

void AAvoidGameMode::StartMission()
{
	MissionManager->StartMission();
}

void AAvoidGameMode::Metric()
{

	MetricManager->Init(GetWorld());
	FString out = MetricManager->GetMetricResults();
	UE_LOG(LogAvoid, Warning, TEXT("resulting jsonString -> %s"), *out);
}

void AAvoidGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MissionManager->Tick(DeltaSeconds);

	time += DeltaSeconds;
	//if (FMath::Fmod(time, 10) < 0.01) {
	//	UE_LOG(LogAvoid, Warning, TEXT("%d"), result.IsReady())
	//}
	//auto resul/*t = DroneManager->SetDroneZ(time * -1);
	//result.*/
}