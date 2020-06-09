#include "Avoid/Client.h"
#include "rpc/RPCClient.h"

namespace avoid{

    class Client::ClientImplementation {
    public:
        ClientImplementation(const std::string &ip, uint16_t port) : rpcClient(ip, port) {};

        std::string GetCurrentMap() {
            return rpcClient.GetCurrentMap();
        }

        std::vector<std::string> GetAvailableMaps() {
            return rpcClient.GetAvailableMaps();
        };

        void LoadMap(const std::string& mapName) {
            return rpcClient.LoadMap(mapName);
        };

        void ReloadMap() {
            return rpcClient.ReloadMap();
        };


    private:
        RPCClient rpcClient;

    };

    Client::Client(const std::string &ip, int port) : clientImplementation(std::make_unique<ClientImplementation>(ip, port)) {};
    Client::~Client() = default;

    std::string Client::GetCurrentMap() {
        return clientImplementation->GetCurrentMap();
    };

    std::vector<std::string> Client::GetAvailableMaps() {
        return clientImplementation->GetAvailableMaps();
    };

    void Client::LoadMap(const std::string& mapName) {
        return clientImplementation->LoadMap(mapName);
    };

    void Client::ReloadMap() {
        return clientImplementation->ReloadMap();
    };

    // std::string Client::GetClientVersion();
    // std::string Client::GetServerVersion();
    // std::vector<std::string> Client::GetAvailableMaps();
    // void Client::ReloadMap();
    // void Client::LoadWorld(const std::string &mapName);

}

