#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AvoidLevelManager.generated.h"

UCLASS()
class AVOID_API UAvoidLevelManager : public UObject
{
	GENERATED_BODY()

public:
	//User can call this with either the map name or the entire reference.
	UFUNCTION(BlueprintCallable)
	bool LoadMap(const FString &Map, bool& success);

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAvailableMaps();

	UFUNCTION(BlueprintCallable)
	FString GetCurrentMap();

	UFUNCTION(BlueprintCallable)
	void GetProceduralLocations(TArray<FString> &locations);

	UFUNCTION(BlueprintCallable)
	void SetProceduralLocation(const FString &location, float radius, int seed);

	AActor* GetActiveProceduralLocation() const { return ActiveProceduralLocation; };

private:
	
	UPROPERTY()
	AActor* ActiveProceduralLocation = nullptr;
};
