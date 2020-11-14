#include "AvoidMetricManager.h"
#include "Kismet/GameplayStatics.h"

UAvoidMetricManager::UAvoidMetricManager()
{
}

void UAvoidMetricManager::Init(UWorld *world)
{
	checkf(world != nullptr, TEXT("World is nullptr."));
	World = world;

	TArray<AActor *> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AMetricBase::StaticClass(), AllActors);
	Metrics.Empty();
	for (auto actor : AllActors)
	{
		Metrics.Add(Cast<AMetricBase>(actor));
	}
}

void UAvoidMetricManager::InitMetrics(const FVector &StartLocation, const FVector &EndLocation)
{
	for (auto metric : Metrics)
	{
		metric->Init(StartLocation, EndLocation);
	}
}
void UAvoidMetricManager::StartMetrics()
{
	for (auto metric : Metrics)
	{
		metric->Start();
	}
}

void UAvoidMetricManager::StopMetrics()
{
	for (auto metric : Metrics)
	{
		metric->Stop();
	}
}
void UAvoidMetricManager::ResetMetrics()
{
	for (auto metric : Metrics)
	{
		metric->Reset();
	}
}

FString UAvoidMetricManager::GetMetricResults()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	for (auto metric : Metrics)
	{
		JsonObject->SetObjectField(metric->GetMetricName(), metric->SerializeResults());
	}

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return OutputString;
}