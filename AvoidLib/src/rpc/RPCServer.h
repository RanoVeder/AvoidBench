#pragma once

#include <rpc/server.h>
#include <functional>

class RPCServer{
public:
    RPCServer(uint16_t port);

    void Bind_GetCurrentMap(std::function<std::string()> callback);
    void GetAvailableMaps(std::function<std::vector<std::string>()> callback);
    void LoadMap(std::function<void(const std::string&)> callback);
    void ReloadMap(std::function<void()> callback);

private:
    rpc::server server;
};