#include "AvoidGameInstance.h"
#include "AvoidGameMode.h"

UAvoidGameInstance::UAvoidGameInstance(const FObjectInitializer &object)
{
	LevelManager = CreateDefaultSubobject<UAvoidLevelManager>(TEXT("AvoidLevelManager"));
	server = new Server();
}

UAvoidGameInstance::~UAvoidGameInstance()
{
	delete server;
}

void UAvoidGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogAvoid, Warning, TEXT("Init"))
};

void UAvoidGameInstance::OnStart()
{
	Super::OnStart();
	server->Start(8090);
	UE_LOG(LogAvoid, Warning, TEXT("ONSTART"))
};

void UAvoidGameInstance::Shutdown()
{
	server->Stop();
	Super::Shutdown();
};

void UAvoidGameInstance::LoadComplete(const float LoadTime, const FString &MapName)
{
	Super::LoadComplete(LoadTime, MapName);
	UE_LOG(LogAvoid, Warning, TEXT("Load Complete"));
	server->WorldLoadingFinished();
}

void UAvoidGameInstance::OnWorldChanged(UWorld *OldWorld, UWorld *NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	UE_LOG(LogAvoid, Warning, TEXT("OldWorld: %p,  NewWorld: %p"), OldWorld, NewWorld);
	server->SetWorld(NewWorld);
}
