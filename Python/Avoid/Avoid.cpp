#include <pybind11/pybind11.h>

#include "Client.cpp"

PYBIND11_MODULE(avoid, m)
{
    register_client(m);
}