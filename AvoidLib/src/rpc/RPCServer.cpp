#include "headers/RPCServer.h"
#include "headers/APIDefinition.h"

RPCServer::RPCServer(uint16_t port) : server(port) {};

void RPCServer::BindGetCurrentWorld(std::function<std::string()> callback) {
    server.bind(GET_CURRENT_WORLD, callback);
};

void RPCServer::BindGetAvailableWorlds(std::function<std::vector<std::string>()> callback) {
    server.bind(GET_AVAILABLE_WORLDS, callback);
};

void RPCServer::BindLoadWorld(std::function<void(const std::string&)> callback) {
    server.bind(LOAD_WORLD, callback);
};

void RPCServer::Start(std::size_t worker_threads) {
    server.async_run(worker_threads);
};

void RPCServer::Stop() {
    server.stop();
};


