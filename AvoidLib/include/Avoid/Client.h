#pragma once

#include <string>
#include <vector>
#include <memory>

namespace avoid {
    class Client {
    public:
        Client(const std::string &ip, uint16_t port);
        ~Client();

        std::string GetCurrentWorld();
        std::string GetClientVersion();
        std::string GetServerVersion();
        std::vector<std::string> GetAvailableWorlds();
        void ReloadWorld();
        void LoadWorld(const std::string& mapName);

    private:
        class ClientImplementation;
        std::unique_ptr<ClientImplementation> clientImplementation;
        
    };
}
