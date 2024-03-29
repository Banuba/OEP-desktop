#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>
#include <bnb/player_api/interfaces/types/pixel_buffer_format.hpp>

namespace bnb::player_api
{

    constexpr uint32_t yuv_offset_to_y_coeffs = 0;
    constexpr uint32_t yuv_offset_to_u_coeffs = 8;
    constexpr uint32_t yuv_offset_to_v_coeffs = 4;

    constexpr uint32_t yuv_offset_to_r_coeffs = 0;
    constexpr uint32_t yuv_offset_to_g_coeffs = 4;
    constexpr uint32_t yuv_offset_to_b_coeffs = 8;

    const float* const get_conversion_matrix_from_rgb_to_yuv(bnb::player_api::pixel_buffer_format to_yuv_format, bnb::color_std std, bnb::color_range rng);

    const float* const get_conversion_matrix_from_yuv_to_rgb(bnb::player_api::pixel_buffer_format from_yuv_format, bnb::color_std std, bnb::color_range rng);

    [[nodiscard]] inline int32_t uv_plane_width(int32_t width)
    {
        return (width + 1) / 2;
    }

    [[nodiscard]] inline int32_t uv_plane_height(int32_t height)
    {
        return (height + 1) / 2;
    }

} // namespace bnb::player_api
