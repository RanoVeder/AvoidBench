#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Api/Server.h"
#include "AvoidLevelManager.h"
#include "AvoidGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AVOID_API UAvoidGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UAvoidGameInstance(const FObjectInitializer &object);

	~UAvoidGameInstance();

	UPROPERTY()
	UAvoidLevelManager *LevelManager;

protected:
	virtual void OnWorldChanged(UWorld *OldWorld, UWorld *NewWorld) override;
	virtual void LoadComplete(const float LoadTime, const FString &MapName) override;
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnStart() override;

private:
	Server *server;
};
