#include "RPCServer.h"
#include "APIDefinition.h"

RPCServer::RPCServer(uint16_t port) : server(port) {};

void RPCServer::Bind_GetCurrentMap(std::function<std::string()> callback) {
    server.bind(GET_CURRENT_MAP, callback);
};

void RPCServer::GetAvailableMaps(std::function<std::vector<std::string>()> callback) {
    server.bind(GET_AVAILABLE_MAPS, callback);
};

void RPCServer::LoadMap(std::function<void(const std::string&)> callback) {
    server.bind(GET_CURRENT_MAP, callback);
};

void RPCServer::ReloadMap(std::function<void()> callback) {
    server.bind(GET_CURRENT_MAP, callback);
};

