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
	virtual void BeginDestroy() override;

public:

	TArray<FVector2D> GetObstacleLocations() const { return Locations; };

	UFUNCTION(CallInEditor, BlueprintCallable)
	void Generate();

	UPROPERTY(EditAnywhere)
	TArray<UFoliageType_InstancedStaticMesh *> FoliageTypes;

	UPROPERTY(EditAnywhere)
	AVolume *Volume;

	UPROPERTY(EditAnywhere)
	float Radius = 200;

	UPROPERTY(EditAnywhere)
	int Iterations = 35;

	UPROPERTY(EditAnywhere)
	int RandomSeed = 0;

private:
	bool FoliageTrace(const UWorld *InWorld, FHitResult &OutHit, const FDesiredFoliageInstance &DesiredInstance);
	bool PlaceInstance(const UWorld *InWorld, const UFoliageType *Settings, FFoliageInstance &Inst, const FVector &HitLocation, const FVector &HitNormal);

	bool GenerateArray();
	void GenerateLocations();
	void PlaceInstances();

	void GetGridBox(FVector2D location, int &row, int &column);
	bool IsValid(FVector2D point);

	TArray<int> Grid;
	TArray<FVector2D> Locations;
	TArray<FVector2D> ActivePoints;
	FBox BoundingBox;

	TArray<UFoliageType_InstancedStaticMesh *> OldTypes;
	TArray<UFoliageInstancedStaticMeshComponent *> Instances;

	int Rows;
	int Columns;
	float GridSize;
};
