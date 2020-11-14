#include <pybind11/pybind11.h>

#include "Client.cpp"
#include "World.cpp"

PYBIND11_MODULE(avoid, m)
{
    register_client(m);
    register_world(m);
    register_vehicle(m);
    register_mission(m);
    register_benchmarking_interface(m);
}