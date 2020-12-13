#include <gtest/gtest.h>
#include <avoid/Client.h>

TEST(CLIENT, connect) {
avoid::Client client("localhost", 8090);

auto maps = client.GetAvailableWorlds();
std::cerr << maps.size() << std::endl;
maps = client.GetAvailableWorlds();
std::cerr << maps.size() << std::endl;

}