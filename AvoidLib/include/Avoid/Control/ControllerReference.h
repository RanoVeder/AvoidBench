#pragma once
namespace avoid
{

    enum class ControlType
    {
        THROTTLE_CONTROL,
        RATE_CONTROL,
        ATTITUDE_CONTROL,
        VELOCITY_CONTROL,
        POSITION_CONTROL,
    };

#define CONTROL_TYPE(type)                                           \
    static ControlType GetStaticType() { return ControlType::type; } \
    virtual ControlType GetControlType() const override { return GetStaticType(); }

    class ControllerReference
    {
    public:
        virtual ControlType GetControlType() const = 0;
    };

    class ThrottleControllerReference : public ControllerReference
    {
    public:
        CONTROL_TYPE(THROTTLE_CONTROL)

        float throttle1, throttle2, throttle3, throttle4;
    };

    class RateControllerReference : public ControllerReference
    {
    public:
        CONTROL_TYPE(RATE_CONTROL)

        float rollRate, pitchRate, yawRate, z;
    };

    class AttitudeControllerReference : public ControllerReference
    {
    public:
        CONTROL_TYPE(ATTITUDE_CONTROL)

        float roll, pitch, yaw, z;
    };

    class VelocityControllerReference : public ControllerReference
    {
    public:
        CONTROL_TYPE(VELOCITY_CONTROL)

        float vx, vy, vz, yaw;
    };

    class PositionControllerReference : public ControllerReference
    {
    public:
        CONTROL_TYPE(POSITION_CONTROL)

        float x, y, z, yaw;
    };
} // namespace avoid