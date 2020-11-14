#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
namespace py = pybind11;

#include <Avoid/Client.h>
#include <Avoid/World.h>
#include <Avoid/Benchmarking/BenchmarkingInterface.h>

void register_client(py::module &m)
{

    py::class_<avoid::Client>(m, "Client")
        .def(py::init<std::string &, uint16_t>())
        .def("get_available_worlds", &avoid::Client::GetAvailableWorlds)
        .def("get_current_world", &avoid::Client::GetCurrentWorld)
        .def("load_world", &avoid::Client::LoadWorld)
        .def("reload_world", &avoid::Client::ReloadWorld)
        .def("get_vehicle", &avoid::Client::GetVehicle);
}

void register_world(py::module &m)
{

    py::class_<avoid::World>(m, "World")
        //.def(py::init<std::string&, uint16_t>())
        .def("get_name", &avoid::World::GetName)
        .def("get_procedural_locations", &avoid::World::GetProceduralLocations)
        .def("set_procedural_location", &avoid::World::SetProceduralLocation)
        .def("get_available_missions", &avoid::World::GetAvailableMissions)
        .def("load_mission", &avoid::World::LoadMission);
}

void register_mission(py::module &m)
{

    py::class_<avoid::Mission>(m, "Mission")
        //.def(py::init<std::string&, uint16_t>());
        .def("get_mission_info", &avoid::Mission::GetMissionInfo)
        .def("start", &avoid::Mission::Start);

    py::class_<avoid::MissionInfo>(m, "MissionInfo")
        .def(py::init<>())
        .def_readwrite("start_position", &avoid::MissionInfo::startPosition)
        .def_readwrite("end_position", &avoid::MissionInfo::endPosition)
        .def_readwrite("goal_heading", &avoid::MissionInfo::goalHeading)
        .def_readwrite("mission_time", &avoid::MissionInfo::missionTime);
}

void register_vehicle(py::module &m)
{

    py::class_<avoid::Vehicle>(m, "Vehicle")
        //.def(py::init<std::string&, uint16_t>())
        .def("get_available_cameras", &avoid::Vehicle::GetAvailableCameras)
        .def("get_image", &avoid::Vehicle::GetImage)
        .def("apply_controller_reference", &avoid::Vehicle::ApplyControllerReference)
        .def("get_state", &avoid::Vehicle::GetState);

    py::class_<avoid::ImageResult>(m, "ImageResult")
        .def(py::init<>())
        .def_readwrite("width", &avoid::ImageResult::width)
        .def_readwrite("height", &avoid::ImageResult::height)
        .def_readwrite("data", &avoid::ImageResult::data);

    py::class_<avoid::ControllerReference>(m, "ControllerReference");

    py::class_<avoid::ThrottleControllerReference, avoid::ControllerReference>(m, "ThrottleControllerReference")
        .def(py::init<>())
        .def_readwrite("throttle_1", &avoid::ThrottleControllerReference::throttle1)
        .def_readwrite("throttle_2", &avoid::ThrottleControllerReference::throttle2)
        .def_readwrite("throttle_3", &avoid::ThrottleControllerReference::throttle3)
        .def_readwrite("throttle_4", &avoid::ThrottleControllerReference::throttle4);

    py::class_<avoid::RateControllerReference, avoid::ControllerReference>(m, "RateControllerReference")
        .def(py::init<>())
        .def_readwrite("roll_rate", &avoid::RateControllerReference::rollRate)
        .def_readwrite("pitch_rate", &avoid::RateControllerReference::pitchRate)
        .def_readwrite("yaw_rate", &avoid::RateControllerReference::yawRate)
        .def_readwrite("z", &avoid::RateControllerReference::z);

    py::class_<avoid::AttitudeControllerReference, avoid::ControllerReference>(m, "AttitudeControllerReference")
        .def(py::init<>())
        .def_readwrite("roll", &avoid::AttitudeControllerReference::roll)
        .def_readwrite("pitch", &avoid::AttitudeControllerReference::pitch)
        .def_readwrite("yaw", &avoid::AttitudeControllerReference::yaw)
        .def_readwrite("z", &avoid::AttitudeControllerReference::z);

    py::class_<avoid::VelocityControllerReference, avoid::ControllerReference>(m, "VelocityControllerReference")
        .def(py::init<>())
        .def_readwrite("vx", &avoid::VelocityControllerReference::vx)
        .def_readwrite("vy", &avoid::VelocityControllerReference::vy)
        .def_readwrite("vz", &avoid::VelocityControllerReference::vz)
        .def_readwrite("yaw", &avoid::VelocityControllerReference::yaw);

    py::class_<avoid::PositionControllerReference, avoid::ControllerReference>(m, "PositionControllerReference")
        .def(py::init<>())
        .def_readwrite("x", &avoid::PositionControllerReference::x)
        .def_readwrite("y", &avoid::PositionControllerReference::y)
        .def_readwrite("z", &avoid::PositionControllerReference::z)
        .def_readwrite("yaw", &avoid::PositionControllerReference::yaw);

    py::class_<avoid::VehicleState>(m, "VehicleState")
        .def(py::init<>())
        .def_readwrite("position", &avoid::VehicleState::position)
        .def_readwrite("velocity", &avoid::VehicleState::velocity)
        .def_readwrite("acceleration", &avoid::VehicleState::acceleration)
        .def_readwrite("orientation", &avoid::VehicleState::orientation)
        .def_readwrite("angularRate", &avoid::VehicleState::angularRate)
        .def_readwrite("angularAcceleration", &avoid::VehicleState::angularAcceleration);
}

class PyBenchmarkingInterface : public avoid::benchmarking::BenchmarkingInterface
{
public:
    PyBenchmarkingInterface(const std::string &ip, uint16_t port) : avoid::benchmarking::BenchmarkingInterface(ip, port){};

    /* Trampoline (need one for each virtual function) */
    virtual void Init() override
    {
        PYBIND11_OVERLOAD_PURE_NAME(void, avoid::benchmarking::BenchmarkingInterface, "_init", Init);
    }

    virtual void Tick(avoid::Vehicle &vehicle, avoid::MissionInfo &missionInfo) override
    {
        PYBIND11_OVERLOAD_PURE_NAME(void, avoid::benchmarking::BenchmarkingInterface, "_tick", Tick, vehicle, missionInfo);
    }

    virtual void Cleanup() override
    {
        PYBIND11_OVERLOAD_PURE_NAME(void, avoid::benchmarking::BenchmarkingInterface, "_cleanup", Cleanup);
    }
};

//https://pybind11.readthedocs.io/en/stable/advanced/classes.html#binding-protected-member-functions
class Publicist : public avoid::benchmarking::BenchmarkingInterface
{
public:
    using avoid::benchmarking::BenchmarkingInterface::Cleanup;
    using avoid::benchmarking::BenchmarkingInterface::Init;
    using avoid::benchmarking::BenchmarkingInterface::Tick;
};

void register_benchmarking_interface(py::module &m)
{

    py::class_<avoid::benchmarking::BenchmarkingDescription>(m, "BenchmarkingDescription")
        .def(py::init<>())
        .def_readwrite("world_name", &avoid::benchmarking::BenchmarkingDescription::worldName)
        .def_readwrite("mission_name", &avoid::benchmarking::BenchmarkingDescription::missionName)
        .def_readwrite("procedural_descriptions", &avoid::benchmarking::BenchmarkingDescription::proceduralDecriptions)
        .def_readwrite("trials", &avoid::benchmarking::BenchmarkingDescription::trials);

    py::class_<avoid::benchmarking::ProceduralGenerationDescription>(m, "ProceduralGenerationDescription")
        .def(py::init<>())
        .def_readwrite("procedural_location", &avoid::benchmarking::ProceduralGenerationDescription::proceduralLocation)
        .def_readwrite("radius", &avoid::benchmarking::ProceduralGenerationDescription::radius);

    py::class_<avoid::benchmarking::BenchmarkingInterface, PyBenchmarkingInterface>(m, "BenchmarkingInterface")
        .def(py::init<std::string &, uint16_t>())
        .def("run", &avoid::benchmarking::BenchmarkingInterface::Run)
        .def("add_benchmarking_description", &avoid::benchmarking::BenchmarkingInterface::AddBenchmarkingDescription)
        .def("_init", &Publicist::Init)
        .def("_tick", &Publicist::Tick)
        .def("_cleanup", &Publicist::Cleanup);
}