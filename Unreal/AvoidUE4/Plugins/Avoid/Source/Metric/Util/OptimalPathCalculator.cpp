#include "OptimalPathCalculator.h"
#include "NavigationSystem.h"

UOptimalPathCalculator::UOptimalPathCalculator()
	: UObject()
{
}

bool UOptimalPathCalculator::Init(UWorld *World, const FVector &start, const FVector &end)
{
	bool val = GetOptimalPathLength(World, start, end);
	if (val)
	{
		Initialised = true;
	}
	else
	{
		Initialised = false;
	}
	return val;
}

float UOptimalPathCalculator::GetPathLength()
{
	if (Initialised)
	{
		return PathResult.Path->GetLength();
	}
	else
	{
		return -1;
	}
}

bool UOptimalPathCalculator::GetOptimalPathLength(UWorld *World, const FVector &start, const FVector &end)
{

	UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (NavSys == nullptr)
	{
		return false;
	}

	ANavigationData *NavData = Cast<ANavigationData>(NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate));
	if (NavData == nullptr)
	{
		return false;
	}

	TSubclassOf<class UNavigationQueryFilter> FilterClass;

	ANavigationData *MyNavData = NULL;
	FNavAgentProperties NavAgentProps = FNavAgentProperties(34, 144);
	NavAgentProps.AgentStepHeight = -1.0;
	NavAgentProps.NavWalkingSearchHeightScale = 0.5;

	MyNavData = NavSys->GetNavDataForProps(NavAgentProps, start);

	FVector QueryingExtent(100, 100, 500);
	FNavLocation NavLocStart;
	bool valid = NavSys->ProjectPointToNavigation(start, NavLocStart, QueryingExtent, MyNavData);

	FNavLocation NavLocEnd;
	valid |= NavSys->ProjectPointToNavigation(end, NavLocEnd, QueryingExtent, MyNavData);

	if (!valid)
	{
		return false;
	}

	FPathFindingQuery Query = FPathFindingQuery(this, *MyNavData, NavLocStart.Location, NavLocEnd.Location, UNavigationQueryFilter::GetQueryFilter(*MyNavData, this, FilterClass));

	{
		FSharedConstNavQueryFilter NavQueryFilter = Query.QueryFilter ? Query.QueryFilter : NavData->GetDefaultQueryFilter();
		FSharedNavQueryFilter NavigationFilterCopy = NavQueryFilter->GetCopy();
		NavigationFilterCopy->SetBacktrackingEnabled(true);
		Query.QueryFilter = NavigationFilterCopy;
	}
	//Apply cost limit factor
	FSharedConstNavQueryFilter NavQueryFilter = Query.QueryFilter ? Query.QueryFilter : NavData->GetDefaultQueryFilter();
	const float HeuristicScale = NavQueryFilter->GetHeuristicScale();
	Query.CostLimit = Query.ComputeCostLimitFromHeuristic(Query.StartLocation, Query.EndLocation, HeuristicScale, 9223372036854775808.0, 0.0);

	EPathFindingMode::Type Mode = EPathFindingMode::Hierarchical;

	PathResult = NavSys->FindPathSync(NavAgentProps, Query, Mode);

	return PathResult.IsSuccessful();
}
