#include "AvoidMissionBase.h"

FVector AAvoidMission::GetGlobalStartLocation()
{
	return GetTransform().TransformPosition(StartLocation);
}

FVector AAvoidMission::GetGlobalFinishLocation()
{
	return GetTransform().TransformPosition(EndLocation);
}