#pragma once

#include <cstdint>

namespace bnb::player_api
{

    enum class content_mode : int32_t
    {
        /**
         * Positions the drawn frame in the center, while maintaining the aspect ratio.
         * With this layout, the entire frame fits into the drawing area.
         * */
        aspect_fit,
        
        /**
         * Positions the drawn frame in the center, while maintaining the aspect ratio.
         * With this layout, the frame is stretched along the largest of the sides, fills the
         * entire drawing area, while the borders of the drawn frame will be cut off.
         * */
        aspect_fill
    }; /* enum class content_mode */

} /* namespace bnb::player_api */
