#pragma once

#include <rpc/msgpack.hpp>

namespace avoid
{
    namespace rpc
    {
        struct MissionInfo
        {
            float startx, starty, startz;
            float endx, endy, endz;
            float goalHeading;
            double time;

            MSGPACK_DEFINE_ARRAY(startx, starty, startz, endx, endy, endz, goalHeading, time)
        };

    } // namespace rpc
} // namespace avoid