#pragma once

#include <rpc/client.h>
#include "headers/types/MissionInfo.h"
#include "headers/types/DroneState.h"
#include "headers/types/ImageResult.h"

namespace avoid
{

    namespace rpc
    {

        class RPCClient
        {
        public:
            RPCClient(const std::string &ip, uint16_t port);

            std::string GetCurrentWorld();
            std::vector<std::string> GetAvailableWorlds();
            bool LoadWorld(const std::string &worldName);

            std::vector<std::string> GetProceduralLocations();
            void SetProceduralLocation(std::string &location, float radius, int seed);

            //image API
            std::vector<std::string> GetAvailableCameras();
            void SetCameraSettings(std::string cameraName, float fov);
            ImageResult GetImage(std::string cameraName);

            //blocking
            void Hover(float z);
            void Takeoff();
            void Land();

            void SetControllerReferenceThrottle(float throttle1, float throttle2, float throttle3, float throttle4);
            void SetControllerReferenceRate(float roll_rate, float pitch_rate, float yaw_rate, float z);
            void SetControllerReferenceAttitude(float roll, float pitch, float yaw, float z);
            void SetControllerReferenceVelocity(float vx, float vy, float vz, float yaw);
            void SetControllerReferencePosition(float x, float y, float z, float yaw);

            void CancelTasks();

            std::vector<std::string> GetAvailableMissions();
            bool LoadMission(std::string missionName);
            void LoadCustomMission(float startPosition, float endPosition);
            void StartFreeFlight();

            int GetMissionState();
            MissionInfo GetMissionInfo();
            std::string GetMetricResults();

            void StartMission();
            void RestartMission();
            void EndMission();

            DroneState GetDroneState();


        private:
            ::rpc::client client;
        };
    } // namespace rpc

} // namespace avoid
