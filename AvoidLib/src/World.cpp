#pragma once
#include "Avoid/World.h"
#include "rpc/RPCClient.h"

//TODO: Validate if world object is still valid (maybe user changed map, but instance still exists)
namespace avoid
{

    World::World(std::shared_ptr<rpc::RPCClient> backend, std::string worldName) : backend(backend), name(worldName){};

    World::~World() = default;

    std::string World::GetName()
    {
        return name;
    }

    std::vector<std::string> World::GetProceduralLocations()
    {
        return backend->GetProceduralLocations();
    }

    void World::SetProceduralLocation(std::string locationName, float radius)
    {
        backend->SetProceduralLocation(locationName, radius);
    };

    std::vector<std::string> World::GetAvailableMissions()
    {
        return backend->GetAvailableMissions();
    }

    Mission World::LoadMission(std::string missionName)
    {
        if(backend->LoadMission(missionName))
        {
            return Mission(backend);
        }
        else
        {
            throw std::exception();
        }
        
    }

    Mission World::LoadCustomMission(float startPosition, float endPosition)
    {
        backend->LoadCustomMission(startPosition, endPosition);
        return Mission(backend);
    }

    void World::StartFreeFlight()
    {
    }

} // namespace avoid
