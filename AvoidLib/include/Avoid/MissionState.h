#pragma once

#include <Eigen/Dense>

namespace avoid
{

    enum class MissionState
    {
        NOTLOADED,
        INITIALISING,
        READY,
        RUNNING,
        ENDED,
        INVALIDSTATE
    };

    struct MissionInfo
    {
        double missionTime;
        Eigen::Vector3f startPosition;
        Eigen::Vector3f endPosition;
        float goalHeading;
    };
} // namespace avoid