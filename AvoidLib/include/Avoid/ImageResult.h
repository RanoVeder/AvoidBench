#pragma once;

#include <numeric>
#include <vector>

namespace avoid
{

    struct ImageResult
    {
        uint32_t width;
        uint32_t height;
        std::vector<uint8_t> data;
    };

} // namespace avoid