#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Common.h"
#include "World.h"
#include "Vehicle.h"

namespace avoid
{

    class Client
    {
    public:
        Client(const std::string &ip, uint16_t port);
        ~Client();

        std::string GetClientVersion();
        std::string GetServerVersion();

        void RunBenchmark();

        World GetCurrentWorld();
        std::vector<std::string> GetAvailableWorlds();
        World ReloadWorld();
        World LoadWorld(const std::string &worldName);

        Vehicle GetVehicle();

        void SetCameraSettings(float fov);

    private:
        std::shared_ptr<rpc::RPCClient> backend;
    };
} // namespace avoid
