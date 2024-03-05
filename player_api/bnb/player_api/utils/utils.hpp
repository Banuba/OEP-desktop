#pragma once

#include <bnb/player_api/interfaces/orientation.hpp>
#include <bnb/player_api/interfaces/output.hpp>

namespace bnb::player_api
{

    const float* const get_static_rotation_mat4(bnb::player_api::orientation orient, bool mirroring);

    void calculate_frame_layout(
            int32_t surface_width,
            int32_t surface_height,
            int32_t frame_width,
            int32_t frame_height,
            bnb::player_api::interfaces::output::content_mode content_mode,
            int32_t& left,
            int32_t& top,
            int32_t& width,
            int32_t& height
    );
    

} /* namespace bnb::player_api */
