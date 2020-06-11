#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <Avoid/Client.h>

void register_client(py::module &m){
    
    py::class_<avoid::Client>(m, "Client")
    .def(py::init<std::string&, uint16_t>())
    .def("get_available_worlds", &avoid::Client::GetAvailableWorlds)
    .def("get_current_world", &avoid::Client::GetCurrentWorld)
    .def("load_world", &avoid::Client::LoadWorld)
    .def("reload_world", &avoid::Client::ReloadWorld);

}