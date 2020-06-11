#pragma once

#include <rpc/client.h>

class RPCClient{
public:
    RPCClient(const std::string& ip, uint16_t port);

    std::string GetCurrentWorld();
    std::vector<std::string> GetAvailableWorlds();
    void LoadWorld(const std::string& worldName);
    

private:
    rpc::client client;
};