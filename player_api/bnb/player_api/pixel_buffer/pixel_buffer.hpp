#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>
#include <functional>

namespace bnb::player_api
{

    class pixel_buffer
        : public bnb::player_api::interfaces::pixel_buffer
    {
    public:
        using plane_deleter = std::function<void(uint8_t*)>;

        static std::shared_ptr<pixel_buffer> create(
            const uint8_t* rgb_plane,
            int32_t rgb_stride,
            int32_t width,
            int32_t height,
            pixel_buffer_format fmt,
            orientation orient,
            bool mirroring,
            plane_deleter deleter
        );

        static std::shared_ptr<pixel_buffer> create(
            const uint8_t* y_plane,
            int32_t y_stride,
            const uint8_t* uv_plane,
            int32_t uv_stride,
            int32_t width,
            int32_t height,
            pixel_buffer_format fmt,
            orientation orient,
            bool mirroring,
            plane_deleter y_deleter,
            plane_deleter uv_deleter
        );

        static std::shared_ptr<pixel_buffer> create(
            const uint8_t* y_plane,
            int32_t y_stride,
            const uint8_t* u_plane,
            int32_t u_stride,
            const uint8_t* v_plane,
            int32_t v_stride,
            int32_t width,
            int32_t height,
            pixel_buffer_format fmt,
            orientation orient,
            bool mirroring,
            plane_deleter y_deleter,
            plane_deleter u_deleter,
            plane_deleter v_deleter
        );
    }; // class pixel_buffer

} // namespace bnb::player_api
