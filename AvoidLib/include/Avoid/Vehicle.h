#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Common.h"

#include "Control/ControllerReference.h"
#include "Control/ControllerSettings.h"
#include "VehicleState.h"
#include "ImageResult.h"


//Wrapping Airlib API in case we ever switch simulator backend
namespace avoid {

    class Vehicle {
    public:
        Vehicle(std::shared_ptr<rpc::RPCClient> backend);
        ~Vehicle();

        VehicleState GetState();

        std::vector<std::string> GetAvailableCameras();
        ImageResult GetImage(std::string &cameraName);

        void Takeoff();
        void Land();
        void Hover();

        //How to get this from airsim apI?
        ControllerSettings GetControllerSettings();
        void UpdateControllerSettings(ControllerSettings &settings);

        void ApplyControllerReference(ControllerReference &reference);

    private:
        std::shared_ptr<rpc::RPCClient> backend;        
    };
}
