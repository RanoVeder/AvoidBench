#pragma once
#include "Avoid/Vehicle.h"
#include "rpc/RPCClient.h"
#include <chrono>
#include <iostream>

//TODO: Validate if world object is still valid (maybe user changed map, but instance still exists)
namespace avoid
{

    Vehicle::Vehicle(std::shared_ptr<rpc::RPCClient> backend) : backend(backend){};

    Vehicle::~Vehicle() = default;

    VehicleState Vehicle::GetState()
    {
        auto state = backend->GetDroneState();

        VehicleState vState;
        vState.position = Eigen::Vector3f(state.px, state.py, state.pz);
        vState.velocity = Eigen::Vector3f(state.vx, state.vy, state.vz);
        vState.acceleration = Eigen::Vector3f(state.ax, state.ay, state.az);
        vState.orientation = Eigen::Vector3f(state.phi, state.theta, state.psi);
        vState.angularRate = Eigen::Vector3f(state.p, state.q, state.r);
        vState.angularAcceleration = Eigen::Vector3f(state.pdot, state.qdot, state.rdot);

        return vState;
    }

    std::vector<std::string> Vehicle::GetAvailableCameras()
    {
        return backend->GetAvailableCameras();
    }

    ImageResult Vehicle::GetImage(std::string &cameraName)
    {
        ImageResult result;
    //    auto start = std::chrono::steady_clock::now();
        auto a =  backend->GetImage(cameraName);
    //    auto end = std::chrono::steady_clock::now();
        result.width = a.width;
        result.height = a.height;
        result.data = a.data;

    // std::chrono::duration<double> elapsed_seconds = end-start;
    // std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
        return result;
    }
    
    void Vehicle::Takeoff()
    {
        backend->Takeoff();
    }

    void Vehicle::Land()
    {
        backend->Land();
    }

    void Vehicle::Hover()
    {
        backend->Hover(5);
    }

    ControllerSettings Vehicle::GetControllerSettings()
    {
        return ControllerSettings();
    }

    void Vehicle::UpdateControllerSettings(ControllerSettings &settings)
    {

    }

    void Vehicle::ApplyControllerReference(ControllerReference &reference)
    {
        switch (reference.GetControlType())
        {
        case ControlType::THROTTLE_CONTROL:
        {

            ThrottleControllerReference &throttleRef = dynamic_cast<ThrottleControllerReference &>(reference);
            backend->SetControllerReferenceThrottle(throttleRef.throttle1, throttleRef.throttle2, throttleRef.throttle3, throttleRef.throttle4);
            break;
        }
        case ControlType::RATE_CONTROL:
        {

            RateControllerReference &rateRef = dynamic_cast<RateControllerReference &>(reference);
            backend->SetControllerReferenceRate(rateRef.rollRate, rateRef.pitchRate, rateRef.pitchRate, rateRef.z);
            break;
        }
        case ControlType::ATTITUDE_CONTROL:
        {

            AttitudeControllerReference &attitudeRef = dynamic_cast<AttitudeControllerReference &>(reference);
            backend->SetControllerReferenceAttitude(attitudeRef.roll, attitudeRef.pitch, attitudeRef.yaw, attitudeRef.z);
            break;
        }
        case ControlType::VELOCITY_CONTROL:
        {

            VelocityControllerReference &velocityRef = dynamic_cast<VelocityControllerReference &>(reference);
            backend->SetControllerReferenceVelocity(velocityRef.vx, velocityRef.vy, velocityRef.vz, velocityRef.yaw);
            break;
        }
        case ControlType::POSITION_CONTROL:
        {

            PositionControllerReference &positionRef = dynamic_cast<PositionControllerReference &>(reference);
            backend->SetControllerReferencePosition(positionRef.x, positionRef.y, positionRef.z, positionRef.yaw);
            break;
        }
        }
    }

} // namespace avoid
