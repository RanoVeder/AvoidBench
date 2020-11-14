#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "InstancedFoliage.h"
#include "PoissonDisc.generated.h"

UCLASS()
class AVOID_API APoissonDisc : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APoissonDisc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void Generate();

	UPROPERTY(EditAnywhere)
	TArray<UFoliageType_InstancedStaticMesh *> FoliageTypes;

	UPROPERTY(EditAnywhere)
	AVolume *volume;

	UPROPERTY(EditAnywhere)
	float radius = 200;

	UPROPERTY(EditAnywhere)
	int iterations = 35;

	UPROPERTY(EditAnywhere)
	int randomSeed = 0;

private:
	bool FoliageTrace(const UWorld *InWorld, FHitResult &OutHit, const FDesiredFoliageInstance &DesiredInstance);
	bool PlaceInstance(const UWorld *InWorld, const UFoliageType *Settings, FFoliageInstance &Inst, const FVector &HitLocation, const FVector &HitNormal);

	bool GenerateArray();
	void GenerateLocations();
	void PlaceInstances();

	void GetGridBox(FVector2D location, int &row, int &column);
	bool IsValid(FVector2D point);

	TArray<int> grid;
	TArray<FVector2D> locations;
	TArray<FVector2D> activePoints;
	FBox boundingBox;

	TArray<UFoliageType_InstancedStaticMesh *> oldTypes;
	TArray<UFoliageInstancedStaticMeshComponent *> instances;
	int counter = 0;

	int currentIndex;
	int rows;
	int columns;
	float gridSize;
};
