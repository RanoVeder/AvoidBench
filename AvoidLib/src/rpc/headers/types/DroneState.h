#pragma once

#include <rpc/msgpack.hpp>
namespace avoid
{
    namespace rpc
    {
        struct DroneState
        {
            float px, py, pz;
            float vx, vy, vz;
            float ax, ay, az;
            float phi, theta, psi;
            float p, q, r;
            float pdot, qdot, rdot;
            MSGPACK_DEFINE_ARRAY(px, py, pz, vx, vy, vz, ax, ay, az, phi, theta, psi, p, q, r, pdot, qdot, rdot)
        };
    } // namespace rpc
} // namespace avoid