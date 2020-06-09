#pragma once

#include <rpc/client.h>

class RPCClient{
public:
    RPCClient(const std::string& ip, uint16_t port);

    std::string GetCurrentMap();
    std::vector<std::string> GetAvailableMaps();
    void LoadMap(const std::string& mapName);
    void ReloadMap();


private:
    rpc::client client;
};