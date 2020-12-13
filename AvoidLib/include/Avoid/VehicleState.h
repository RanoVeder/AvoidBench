#pragma once;

#include <Eigen/Dense>

namespace avoid
{

    struct VehicleState
    {
        Eigen::Vector3f position;
        Eigen::Vector3f velocity;
        Eigen::Vector3f acceleration;
        Eigen::Vector3f orientation;
        Eigen::Vector3f angularRate;
        Eigen::Vector3f angularAcceleration;
    };

} // namespace avoid