#include "AvoidLevelManager.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "Kismet/GameplayStatics.h"
#include "Procedural/PoissonDisc.h"

bool UAvoidLevelManager::LoadMap(const FString &MapName)
{

	FString FinalReference = MapName;
	bool ValidMapName = false;

	if (MapName.StartsWith("/Game"))
	{
		if (!MapName.EndsWith(".umap"))
		{
			FinalReference += ".umap";
		}
		FinalReference = FinalReference.Replace(TEXT("/Game/"), *FPaths::ProjectContentDir());
		if (IFileManager::Get().FileExists(*FinalReference))
		{
			ValidMapName = true;
		}
	}
	else
	{
		if (!MapName.Contains("/"))
		{
			TArray<FString> FoundFiles;
			FinalReference += FString(".umap");
			IFileManager::Get().FindFilesRecursive(FoundFiles, *FPaths::ProjectContentDir(), *FinalReference, true, false, false);
			if (FoundFiles.Num() != 0)
			{
				FString Left, Right;
				FinalReference = FoundFiles[0];
				FinalReference.Split("/Content", &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromStart);
				FinalReference = "/Game" + Right;
				FinalReference.RemoveFromEnd(".umap");
				ValidMapName = true;
			}
		}
	}

	if (ValidMapName)
	{
		UE_LOG(LogAvoid, Warning, TEXT("Loading new level: %s"), *FinalReference);
		UGameplayStatics::OpenLevel(GetWorld(), *FinalReference);
	}
	return ValidMapName;
}

TArray<FString> UAvoidLevelManager::GetAvailableMaps()
{
	TArray<FString> FoundFiles, FinalFiles;
	IFileManager::Get().FindFilesRecursive(FoundFiles, *FPaths::ProjectContentDir(), TEXT("*.umap"), true, false, false);
	FoundFiles.RemoveAll([](const FString &Name) { return Name.Contains("NaturePackage"); });

	for (int i = 0; i < FoundFiles.Num(); i++)
	{
		FString Left, Right;
		FoundFiles[i].Split("/Content", &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		Right.RemoveFromEnd(".umap");
		FinalFiles.Add("/Game" + Right);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalFiles[i]);
	}
	return FinalFiles;
}

FString UAvoidLevelManager::GetCurrentMap()
{

	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	return LevelName;
}

void UAvoidLevelManager::GetProceduralLocations(TArray<FString> &locations)
{
	TArray<AActor *> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APoissonDisc::StaticClass(), Actors);
	for (int i = 0; i < Actors.Num(); i++)
	{
		locations.Add(Actors[i]->GetName());
	}
}

void UAvoidLevelManager::SetProceduralLocation(const FString &location, float radius, int seed)
{
	TArray<AActor *> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APoissonDisc::StaticClass(), Actors);
	for (int i = 0; i < Actors.Num(); i++)
	{
		if (location == Actors[i]->GetName())
		{
			APoissonDisc *pd = Cast<APoissonDisc>(Actors[i]);
			pd->Radius = radius;
			pd->RandomSeed = seed;
			pd->Generate();
			ActiveProceduralLocation = Actors[i];
			break;
		};
	}
};
