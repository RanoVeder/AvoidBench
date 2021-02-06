#include "RPCClient.h"
#include "headers/APIDefinition.h"
#include <iostream>

namespace avoid
{

    namespace rpc
    {
        RPCClient::RPCClient(const std::string &ip, uint16_t port) : client(ip, port){
            client.set_timeout(10000);
        };

        std::string RPCClient::GetCurrentWorld()
        {
            return client.call(GET_CURRENT_WORLD).as<std::string>();
        };

        std::vector<std::string> RPCClient::GetAvailableWorlds()
        {
            return client.call(GET_AVAILABLE_WORLDS).as<std::vector<std::string>>();
        };

        bool RPCClient::LoadWorld(const std::string &mapName)
        {
            return client.call(LOAD_WORLD, mapName).as<bool>();
        };

        std::vector<std::string> RPCClient::GetProceduralLocations()
        {
            return client.call(GET_PROCEDURAL_LOCATIONS).as<std::vector<std::string>>();
        }

        void RPCClient::SetProceduralLocation(std::string &location, float radius, int seed)
        {
            client.call(SET_PROCEDURAL_LOCATION, location, radius, seed);
        }

        std::vector<std::string> RPCClient::GetAvailableCameras()
        {
            return client.call(GET_AVAILABLE_CAMERAS).as<std::vector<std::string>>();
        }

        void RPCClient::SetCameraSettings(std::string cameraName, float fov)
        {
            client.call(SET_CAMERA_SETTINGS);
        }

        ImageResult RPCClient::GetImage(std::string cameraName)
        {
            return client.call(GET_IMAGE, cameraName).as<ImageResult>();
        }

        void RPCClient::Hover(float z)
        {
            client.call(HOVER, z);
        }

        void RPCClient::Takeoff()
        {
            client.call(TAKEOFF);
        }

        void RPCClient::Land()
        {
            client.call(LAND);
        }

        void RPCClient::SetControllerReferenceThrottle(float throttle1, float throttle2, float throttle3, float throttle4)
        {
            client.call(SET_REFERENCE_THRUST, throttle1, throttle2, throttle3, throttle4);
        }

        void RPCClient::SetControllerReferenceRate(float roll_rate, float pitch_rate, float yaw_rate, float z)
        {
            client.call(SET_REFERENCE_RATE, roll_rate, pitch_rate, yaw_rate, z);
        }

        void RPCClient::SetControllerReferenceAttitude(float roll, float pitch, float yaw, float z)
        {
            client.call(SET_REFERENCE_ATTITUDE, roll, pitch, yaw, z);
        }

        void RPCClient::SetControllerReferenceVelocity(float vx, float vy, float vz, float yaw)
        {
            client.call(SET_REFERENCE_VELOCITY, vx, vy, vz, yaw);
        }

        void RPCClient::SetControllerReferencePosition(float x, float y, float z, float yaw)
        {
            client.call(SET_REFERENCE_POSITION, x, y, z, yaw);
        }

        void RPCClient::CancelTasks()
        {
            client.call(CANCEL_TASK);
        }

        std::vector<std::string> RPCClient::GetAvailableMissions()
        {
            return client.call(GET_AVAILABLE_MISSIONS).as<std::vector<std::string>>();
        }

        bool RPCClient::LoadMission(std::string missionName)
        {
            return client.call(LOAD_MISSION, missionName).as<bool>();
        }

        void RPCClient::LoadCustomMission(float startPosition, float endPosition)
        {
            client.call(LOAD_CUSTOM_MISSION, startPosition, endPosition);
        }

        void RPCClient::StartFreeFlight()
        {
            client.call(START_FREE_FLIGHT);
        }

        int RPCClient::GetMissionState()
        {
            return client.call(GET_MISSION_STATE).as<int>();
        }

        MissionInfo RPCClient::GetMissionInfo()
        {
            return client.call(GET_MISSION_INFO).as<MissionInfo>();
        }

        std::string RPCClient::GetMetricResults()
        {
            return client.call(GET_METRIC_RESULTS).as<std::string>();
        }

        void RPCClient::StartMission()
        {
            client.call(START_MISSION);
        }

        void RPCClient::RestartMission()
        {
            client.call(RESTART_MISSION);
        }

        void RPCClient::EndMission()
        {
            client.call(STOP_MISSION);
        }

        DroneState RPCClient::GetDroneState()
        {
            return client.call(GET_DRONE_STATE).as<DroneState>();
        }

    } // namespace rpc
} // namespace avoid
