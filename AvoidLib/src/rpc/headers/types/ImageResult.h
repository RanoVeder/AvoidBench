#pragma once

#include <rpc/msgpack.hpp>
#include <vector>

namespace avoid
{
    namespace rpc
    {
        struct ImageResult
        {
            uint32_t width;
            uint32_t height;
            std::vector<uint8_t> data;
            MSGPACK_DEFINE_ARRAY(width, height, data)
        };
    
    } // namespace rpc
} // namespace avoid