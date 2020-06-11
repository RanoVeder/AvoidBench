#include "Avoid/Client.h"
#include "rpc/RPCClient.h"

namespace avoid{

    class Client::ClientImplementation {
    public:
        ClientImplementation(const std::string &ip, uint16_t port) : rpcClient(ip, port) {};

        std::string GetCurrentWorld() {
            return rpcClient.GetCurrentWorld();
        }

        std::vector<std::string> GetAvailableWorlds() {
            return rpcClient.GetAvailableWorlds();
        };

        void LoadWorld(const std::string& worldName) {
            return rpcClient.LoadWorld(worldName);
        };

    private:
        RPCClient rpcClient;

    };

    Client::Client(const std::string &ip, uint16_t port) : clientImplementation(std::make_unique<ClientImplementation>(ip, port)) {};
    Client::~Client() = default;

    std::string Client::GetCurrentWorld() {
        return clientImplementation->GetCurrentWorld();
    };

    std::vector<std::string> Client::GetAvailableWorlds() {
        return clientImplementation->GetAvailableWorlds();
    };

    void Client::LoadWorld(const std::string& worldName) {
        return clientImplementation->LoadWorld(worldName);
    };

    void Client::ReloadWorld() {
        std::string currentWorld = GetCurrentWorld();
        return clientImplementation->LoadWorld(currentWorld);
    };

}

