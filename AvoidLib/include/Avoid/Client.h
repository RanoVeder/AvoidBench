#pragma once

#include <string>
#include <vector>
#include <memory>

namespace avoid {
    class Client {
    public:
        Client(const std::string &ip, int port);
        ~Client();

        std::string GetCurrentMap();
        std::string GetClientVersion();
        std::string GetServerVersion();
        std::vector<std::string> GetAvailableMaps();
        void ReloadMap();
        void LoadMap(const std::string& mapName);

    private:
        class ClientImplementation;
        std::unique_ptr<ClientImplementation> clientImplementation;
        
    };
}
