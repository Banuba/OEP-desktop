#include <bnb/player_api/utils/utils.hpp>

namespace bnb::player_api
{

    const float* const get_static_rotation_mat4(bnb::player_api::orientation orient, bool mirroring)
    {
        // clang-format off
        static const float matrices[] = {
            // mirroring 0, 0 degrees
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 90 degrees
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 180 degrees
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 270 degrees
            0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,

            // mirroring 0, 0 degrees
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 90 degrees
            0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 180 degrees
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 270 degrees
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        // clang-format on
        return matrices + ((static_cast<int32_t>(orient) / 90) + static_cast<int32_t>(mirroring) * 4) * 16;
    }
    
    /**
     * Calculates the coordinates of a rectangular drawing area according to the selected logic.
     */
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
    ) {
        auto width_scale = static_cast<float>(surface_width) / static_cast<float>(frame_width);
        auto height_scale = static_cast<float>(surface_height) / static_cast<float>(frame_height);
        auto scale = content_mode == bnb::player_api::interfaces::output::content_mode::aspect_fill
                ? (width_scale > height_scale ? width_scale : height_scale) // max
                : (width_scale < height_scale ? width_scale : height_scale); // min
        width = static_cast<int32_t>(static_cast<float>(frame_width) * scale);
        height = static_cast<int32_t>(static_cast<float>(frame_height) * scale);
        left = (surface_width - width) / 2;
        top = (surface_height - height) / 2;
    }

} /* namespace bnb::player_api */
