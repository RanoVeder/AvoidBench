#pragma once

#include <rpc/server.h>
#include <functional>

class RPCServer{
public:
    RPCServer(uint16_t port);

    void BindGetCurrentWorld(std::function<std::string()> callback);
    void BindGetAvailableWorlds(std::function<std::vector<std::string>()> callback);
    void BindLoadWorld(std::function<void(const std::string&)> callback);
    void Start(std::size_t worker_threads);
    void Stop();

private:
    rpc::server server;
};