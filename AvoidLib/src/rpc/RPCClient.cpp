#include "RPCClient.h"
#include "APIDefinition.h"

RPCClient::RPCClient(const std::string& ip, uint16_t port) : client(ip, port) {
};

std::string RPCClient::GetCurrentMap() {
    return client.call(GET_CURRENT_MAP).as<std::string>();
};

std::vector<std::string> RPCClient::GetAvailableMaps() {
    return client.call(GET_AVAILABLE_MAPS).as<std::vector<std::string>>();
};

void RPCClient::LoadMap(const std::string& mapName) {
    client.call(LOAD_MAP, mapName);
};

void RPCClient::ReloadMap() {
    client.call(RELOAD_MAP);
};
