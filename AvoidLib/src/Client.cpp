#include "Avoid/Client.h"
#include "rpc/RPCClient.h"

namespace avoid
{

    Client::Client(const std::string &ip, uint16_t port) : backend(std::make_shared<rpc::RPCClient>(ip, port)){};
    Client::~Client() {
        backend->CancelTasks();
    };

    World Client::GetCurrentWorld()
    {
        std::string name = backend->GetCurrentWorld();
        return World(backend, name);
    };

    std::vector<std::string> Client::GetAvailableWorlds()
    {
        return backend->GetAvailableWorlds();
    };

    World Client::LoadWorld(const std::string &worldName)
    {
        bool success = backend->LoadWorld(worldName);
        if (success)
        {
            return World(backend, worldName);
        }
        throw std::exception();
    };

    World Client::ReloadWorld()
    {
        std::string name = backend->GetCurrentWorld();
        return LoadWorld(name);
    };


    Vehicle Client::GetVehicle()
    {
        return Vehicle(backend);
    };

} // namespace avoid
