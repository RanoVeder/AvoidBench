#include "AvoidGameInstance.h"
#include "AvoidGameMode.h"

UAvoidGameInstance::UAvoidGameInstance(const FObjectInitializer &object)
{
	LevelManager = CreateDefaultSubobject<UAvoidLevelManager>(TEXT("AvoidLevelManager"));
	RPCServer = new Server();
}

UAvoidGameInstance::~UAvoidGameInstance()
{
	delete RPCServer;
}

void UAvoidGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogAvoid, Warning, TEXT("Init"))
};

void UAvoidGameInstance::OnStart()
{
	Super::OnStart();
	RPCServer->Start(8090);
	UE_LOG(LogAvoid, Warning, TEXT("ONSTART"))
};

void UAvoidGameInstance::Shutdown()
{
	RPCServer->Stop();
	Super::Shutdown();
};

void UAvoidGameInstance::LoadComplete(const float LoadTime, const FString &MapName)
{
	Super::LoadComplete(LoadTime, MapName);
	UE_LOG(LogAvoid, Warning, TEXT("Load Complete"));
	RPCServer->WorldLoadingFinished();
}

void UAvoidGameInstance::OnWorldChanged(UWorld *OldWorld, UWorld *NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	UE_LOG(LogAvoid, Warning, TEXT("OldWorld: %p,  NewWorld: %p"), OldWorld, NewWorld);
	RPCServer->SetWorld(NewWorld);
}
