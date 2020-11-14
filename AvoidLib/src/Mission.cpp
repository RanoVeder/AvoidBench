#pragma once
#include "Avoid/Mission.h"
#include "rpc/RPCClient.h"
#include <iostream>

//TODO: Validate if world object is still valid (maybe user changed map, but instance still exists)
namespace avoid
{

    Mission::Mission(std::shared_ptr<rpc::RPCClient> backend) : backend(backend){};

    Mission::~Mission() = default;

    const MissionState Mission::GetMissionState()
    {
        int state = backend->GetMissionState();
        switch (state)
        {
        case 0:
            return MissionState::NOTLOADED;
        case 1:
            return MissionState::INITIALISING;
        case 2:
            return MissionState::READY;
        case 3:
            return MissionState::RUNNING;
        case 4:
            return MissionState::ENDED;
        default:
            return MissionState::INVALIDSTATE;
        }
    }

    const MissionInfo Mission::GetMissionInfo()
    {
        avoid::rpc::MissionInfo info = backend->GetMissionInfo();

        MissionInfo finalInfo;
        finalInfo.startPosition = Eigen::Vector3f(info.startx, info.starty, info.startz);
        finalInfo.endPosition = Eigen::Vector3f(info.endx, info.endy, info.endz);
        finalInfo.missionTime = info.time;
        finalInfo.goalHeading = info.goalHeading;

        return finalInfo;
    }

    const std::string Mission::GetMetricResults()
    {
        std::string results = backend->GetMetricResults();
        return results;
    }

    void Mission::Start()
    {
        backend->StartMission();
    }

    void Mission::Restart()
    {
        backend->RestartMission();
    }

    void Mission::End()
    {
        backend->EndMission();
    }

} // namespace avoid
