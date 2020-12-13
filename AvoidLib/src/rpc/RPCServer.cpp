#include "headers/RPCServer.h"
#include "headers/APIDefinition.h"

namespace avoid
{

    namespace rpc
    {

        RPCServer::RPCServer(uint16_t port) : server(port){};

        void RPCServer::BindGetCurrentWorld(std::function<std::string()> callback)
        {
            server.bind(GET_CURRENT_WORLD, callback);
        };

        void RPCServer::BindGetAvailableWorlds(std::function<std::vector<std::string>()> callback)
        {
            server.bind(GET_AVAILABLE_WORLDS, callback);
        };

        void RPCServer::BindLoadWorld(std::function<bool(const std::string &)> callback)
        {
            server.bind(LOAD_WORLD, callback);
        };

        void RPCServer::BindGetProceduralLocations(std::function<std::vector<std::string>()> callback)
        {
            server.bind(GET_PROCEDURAL_LOCATIONS, callback);
        };

        void RPCServer::BindSetProceduralLocation(std::function<void(const std::string &, float, int)> callback)
        {
            server.bind(SET_PROCEDURAL_LOCATION, callback);
        };

        void RPCServer::BindGetAvailableCameras(std::function<std::vector<std::string>(void)> callback)
        {
            server.bind(GET_AVAILABLE_CAMERAS, callback);
        }

        void RPCServer::BindSetCameraSettings(std::function<void(std::string, float)> callback)
        {
            server.bind(SET_CAMERA_SETTINGS, callback);
        }

        void RPCServer::BindGetImage(std::function<ImageResult(std::string)> callback)
        {
            server.bind(GET_IMAGE, callback);
        }

        void RPCServer::BindHover(std::function<void(float)> callback)
        {
            server.bind(HOVER, callback);
        };

        void RPCServer::BindTakeoff(std::function<void()> callback)
        {
            server.bind(TAKEOFF, callback);
        };

        void RPCServer::BindLand(std::function<void()> callback)
        {
            server.bind(LAND, callback);
        };

        void RPCServer::BindSetControllerReferenceThrottle(std::function<void(float, float, float, float)> callback)
        {
            server.bind(SET_REFERENCE_THRUST, callback);
        };

        void RPCServer::BindSetControllerReferenceRate(std::function<void(float, float, float, float)> callback)
        {
            server.bind(SET_REFERENCE_RATE, callback);
        };

        void RPCServer::BindSetControllerReferenceAttitude(std::function<void(float, float, float, float)> callback)
        {
            server.bind(SET_REFERENCE_ATTITUDE, callback);
        };

        void RPCServer::BindSetControllerReferenceVelocity(std::function<void(float, float, float, float)> callback)
        {
            server.bind(SET_REFERENCE_VELOCITY, callback);
        };

        void RPCServer::BindSetControllerReferencePosition(std::function<void(float, float, float, float)> callback)
        {
            server.bind(SET_REFERENCE_POSITION, callback);
        };

        void RPCServer::BindCancelTasks(std::function<void()> callback)
        {
            server.bind(CANCEL_TASK, callback);
        }

        void RPCServer::BindGetAvailableMissions(std::function<std::vector<std::string>()> callback)
        {
            server.bind(GET_AVAILABLE_MISSIONS, callback);
        }

        void RPCServer::BindLoadMission(std::function<bool(std::string)> callback)
        {
            server.bind(LOAD_MISSION, callback);
        }

        void RPCServer::BindLoadCustomMission(std::function<void(float, float)> callback)
        {
            server.bind(LOAD_CUSTOM_MISSION, callback);
        }

        void RPCServer::BindStartFreeFlight(std::function<void()> callback)
        {
            server.bind(START_FREE_FLIGHT, callback);
        }

        void RPCServer::BindGetMissionState(std::function<int()> callback)
        {
            server.bind(GET_MISSION_STATE, callback);
        }

        void RPCServer::BindGetMetricResults(std::function<std::string()> callback)
        {
            server.bind(GET_METRIC_RESULTS, callback);
        }

        void RPCServer::BindStartMission(std::function<void()> callback)
        {
            server.bind(START_MISSION, callback);
        }

        void RPCServer::BindRestartMission(std::function<void()> callback)
        {
            server.bind(RESTART_MISSION, callback);
        }

        void RPCServer::BindEndMission(std::function<void()> callback)
        {
            server.bind(STOP_MISSION, callback);
        }

        void RPCServer::BindGetDroneState(std::function<DroneState()> callback)
        {
            server.bind(GET_DRONE_STATE, callback);
        }

        void RPCServer::BindGetMissionInfo(std::function<MissionInfo()> callback)
        {
            server.bind(GET_MISSION_INFO, callback);
        }

        void RPCServer::Start(std::size_t worker_threads)
        {
            server.async_run(worker_threads);
        };

        void RPCServer::Stop()
        {
            server.stop();
        };

    } // namespace rpc
} // namespace avoid
