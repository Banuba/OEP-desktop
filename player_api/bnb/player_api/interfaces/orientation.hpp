#pragma once

#include <cstdint>
#include <stdexcept>

namespace bnb::player_api
{
    
    enum class orientation : int32_t
    {
        up = 0,     /* 0 degrees */
        left = 90,  /* 90 degrees */
        down = 180, /* 180 degrees */
        right = 270 /* 270 degrees */
    }; /* enum class orientation */

    inline const orientation degrees_to_orientation(int32_t degrees)
    {
        degrees = degrees % 360;
        if (degrees == 0) {
            return orientation::up;
        } else if (degrees == 90) {
            return orientation::left;
        } else if (degrees == 180) {
            return orientation::down;
        } else if (degrees == 270) {
            return orientation::right;
        }
        throw std::invalid_argument("unknown degrees value: " + degrees);
    }

} /* namespace bnb::player_api */
