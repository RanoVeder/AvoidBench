#pragma once

#include <vector>

namespace avoid
{

    class ControllerSettings
    {
        //Ensure this class can only be obtained via Vehicle's GetControllerSettings, while still being visible to the intellisense of users
        friend class Vehicle;

    private:
        ControllerSettings(){};

    public:
        std::vector<float> p_rate, i_rate, d_rate;
        std::vector<float> p_attitude, i_attitude, d_attitude;
        std::vector<float> p_velocity, i_velocity, d_velocity;
        std::vector<float> p_position, i_position, d_position;
    };
} // namespace avoid