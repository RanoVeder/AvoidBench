#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetricBase.generated.h"

UCLASS()
class AVOID_API AMetricBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMetricBase();

	virtual bool Init(const FVector &StartLocation, const FVector &EndLocation);
	virtual void Start() PURE_VIRTUAL(, );
	virtual void Stop() PURE_VIRTUAL(, );
	virtual void Restart() PURE_VIRTUAL(, );
	virtual FString GetMetricName() PURE_VIRTUAL(, return TEXT(""););
	virtual TSharedPtr<FJsonObject> SerializeResults() PURE_VIRTUAL(, return MakeShareable(new FJsonObject););

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	FVector StartLocation;
	FVector EndLocation;
};
