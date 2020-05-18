// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "PoissonDisc.generated.h"




UCLASS()
class BENCH_API APoissonDisc : public AActor
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


	UFUNCTION(CallInEditor)
		void Generate();

	UPROPERTY(EditAnywhere)
	TArray<UFoliageType_InstancedStaticMesh*> FoliageTypes;

	UPROPERTY(EditAnywhere)
	AVolume* volume;

	UPROPERTY(EditAnywhere)
	float radius = 1;

	UPROPERTY(EditAnywhere)
	int iterations = 8;

	UPROPERTY(EditAnywhere)
	int randomSeed;



	

private:

	void GenerateArray();
	void GenerateLocations();
	void PlaceInstances();

	void GetGridBox(FVector2D location, int& row, int& column);
	bool IsValid(FVector2D point);

	TArray<int> grid;
	TArray<FVector2D> locations;
	TArray<FVector2D> activePoints;
	FBox boundingBox;

	TArray<UFoliageInstancedStaticMeshComponent* > instances;

	int currentIndex;
	int rows;
	int columns;
	float gridSize;
};
