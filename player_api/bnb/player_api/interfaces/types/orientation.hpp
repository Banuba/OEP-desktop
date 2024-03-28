#pragma once

#include <bnb/utils/defs.hpp>

#include <cstdint>

namespace bnb
{
    enum class camera_orientation : unsigned int;
} // namespace bnb

namespace bnb::player_api
{

    // clang-format off
    enum class orientation : int32_t
    {
        up = 0,     // 0 degrees
        left = 90,  // 90 degrees
        down = 180, // 180 degrees
        right = 270 // 270 degrees
    }; // enum class orientation
    // clang-format on

    BNB_EXPORT orientation degrees_to_orientation(int32_t degrees);

    BNB_EXPORT orientation camera_orientation_to_orientation(bnb::camera_orientation orient);

    BNB_EXPORT bnb::camera_orientation orientation_to_camera_orientation(orientation orient);

} // namespace bnb::player_api
