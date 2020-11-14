#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Common.h"
#include "MissionState.h"

//Wrapping Airlib API in case we ever switch simulator backend
namespace avoid {

    class Mission {
    public:
        Mission(std::shared_ptr<rpc::RPCClient> backend);
        ~Mission();

        const MissionState GetMissionState();
        const MissionInfo GetMissionInfo();

        //For now metrics are hardcoded into UE4. Metric results are currently retrieved via the 'Mission' class
        const std::string GetMetricResults();

        void Start();
        void Restart();
        void End();

    private:
        std::shared_ptr<rpc::RPCClient> backend;        
    };
}
