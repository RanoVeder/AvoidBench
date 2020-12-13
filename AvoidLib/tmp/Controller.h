#pragma once

#include <string>
#include <vector>
#include <memory>


//Wrapping Airlib API in case we ever switch simulator backend
namespace avoid {
    class Controller {
    public:
        Controller(Context context);
        ~Controller();

        void Takeoff();
        void Land();
        void Hover();
 
    private:
        class ControllerImplementation;
        std::unique_ptr<ControllerImplementation> controllerImplementation;
        
    };
}
