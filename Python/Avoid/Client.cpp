#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <Avoid/Client.h>

void register_client(py::module &m){
    
    py::class_<avoid::Client>(m, "Client")
    .def(py::init<std::string, uint16_t>())
    //.def("get_available_maps", &get_available_maps)
    .def("get_current_map", &avoid::Client::GetCurrentMap);
    //.def("load_map")
    //.def("reload_map")

}