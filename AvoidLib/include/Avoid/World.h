#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Common.h"

#include "Mission.h"

namespace avoid
{

    class World
    {
    public:
        World(std::shared_ptr<rpc::RPCClient> backend, std::string worldName);
        ~World();

        std::string GetName();

        std::vector<std::string> GetProceduralLocations();
        void SetProceduralLocation(std::string locationName, float radius);

        std::vector<std::string> GetAvailableMissions();
        Mission LoadMission(std::string missionName);
        Mission LoadCustomMission(float startPosition, float endPosition);
        void StartFreeFlight();

    private:
        std::string name;
        std::shared_ptr<rpc::RPCClient> backend;
    };
} // namespace avoid
