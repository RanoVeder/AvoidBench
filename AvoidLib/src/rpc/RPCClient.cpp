#include "RPCClient.h"
#include "headers/APIDefinition.h"
#include <iostream>

RPCClient::RPCClient(const std::string& ip, uint16_t port) : client(ip, port) {
};

std::string RPCClient::GetCurrentWorld() {
    return client.call(GET_CURRENT_WORLD).as<std::string>();
};

std::vector<std::string> RPCClient::GetAvailableWorlds() {
    return client.call(GET_AVAILABLE_WORLDS).as<std::vector<std::string>>();
};

void RPCClient::LoadWorld(const std::string& mapName) {
    client.call(LOAD_WORLD, mapName);
};
