#pragma once

#include "types/DroneState.h"
#include "types/MissionInfo.h"
#include "types/ImageResult.h"
#include <rpc/server.h>
#include "rpc/this_handler.h"
#include <functional>


namespace avoid
{
    namespace rpc
    {
        class ControlCommand;

        class RPCServer
        {
        public:
            RPCServer(uint16_t port);

            void BindGetCurrentWorld(std::function<std::string()> callback);
            void BindGetAvailableWorlds(std::function<std::vector<std::string>()> callback);
            void BindLoadWorld(std::function<bool(const std::string &)> callback);

            void BindGetProceduralLocations(std::function<std::vector<std::string>()> callback);
            void BindSetProceduralLocation(std::function<void(const std::string &, float, int)> callback);

            void BindGetAvailableCameras(std::function<std::vector<std::string>(void)> callback);
            void BindSetCameraSettings(std::function<void(std::string, float)> callback);
            void BindGetImage(std::function<ImageResult(std::string)> callback);

            //dev
            void BindHover(std::function<void(float)> callback);
            void BindTakeoff(std::function<void()> callback);
            void BindLand(std::function<void()> callback);

            void BindSetControllerReferenceThrottle(std::function<void(float, float, float, float)> callback);
            void BindSetControllerReferenceRate(std::function<void(float, float, float, float)> callback);
            void BindSetControllerReferenceAttitude(std::function<void(float, float, float, float)> callback);
            void BindSetControllerReferenceVelocity(std::function<void(float, float, float, float)> callback);
            void BindSetControllerReferencePosition(std::function<void(float, float, float, float)> callback);

            void BindCancelTasks(std::function<void()> callback);

            void BindGetAvailableMissions(std::function<std::vector<std::string>()> callback);
            void BindLoadMission(std::function<bool(std::string)> callback);
            void BindLoadCustomMission(std::function<void(float, float)> callback);
            void BindStartFreeFlight(std::function<void()> callback);

            void BindGetMissionState(std::function<int()> callback);
            void BindGetMissionInfo(std::function<MissionInfo()> callback);
            void BindGetMetricResults(std::function<std::string()> callback);
            void BindStartMission(std::function<void()> callback);
            void BindRestartMission(std::function<void()> callback);
            void BindEndMission(std::function<void()> callback);


            void BindGetDroneState(std::function<DroneState()> callback);

            void Start(std::size_t worker_threads);
            void Stop();

        private:
            ::rpc::server server;
        };
    } // namespace rpc

} // namespace avoid
