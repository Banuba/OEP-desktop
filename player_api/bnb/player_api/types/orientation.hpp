#pragma once

#include <cstdint>

namespace bnb
{
    enum class camera_orientation : unsigned int;
} /* namespace bnb */

namespace bnb::player_api
{

    enum class orientation : int32_t
    {
        up = 0,     /* 0 degrees */
        left = 90,  /* 90 degrees */
        down = 180, /* 180 degrees */
        right = 270 /* 270 degrees */
    }; /* enum class orientation */

    orientation degrees_to_orientation(int32_t degrees);

    orientation camera_orientation_to_orientation(bnb::camera_orientation orient);

} /* namespace bnb::player_api */
