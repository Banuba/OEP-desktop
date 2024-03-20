#pragma once

#include <cstdint>
#include <string>

namespace bnb
{
    struct yuv_format_t;
} // namespace bnb

namespace bnb::interfaces
{
    enum class pixel_format : int;
} // namespace bnb::interfaces

namespace bnb::player_api
{

    /**
     * bt601 and bt709 - two standards for representing color space that use the same
     * image encoding/decoding algorithm. Differ in the encoding / decoding coefficients.
     */
    // clang-format off
    enum class pixel_buffer_format : int32_t
    {
        bpc8_rgb,         // one plane: RGB
        bpc8_bgr,         // one plane: BGR
        bpc8_rgba,        // one plane: RGBA
        bpc8_bgra,        // one plane: BGRA
        bpc8_argb,        // one plane: ARGB
        nv12_bt601_full,  // two planes: first RED, second RG
        nv12_bt601_video, // two planes: first RED, second RG
        nv12_bt709_full,  // two planes: first RED, second RG
        nv12_bt709_video, // two planes: first RED, second RG
        i420_bt601_full,  // three planes: first RED, second RED, third RED
        i420_bt601_video, // three planes: first RED, second RED, third RED
        i420_bt709_full,  // three planes: first RED, second RED, third RED
        i420_bt709_video  // three planes: first RED, second RED, third RED
    }; // enum class pixel_buffer_format
    // clang-format on

    bool pixel_buffer_format_is_bpc8(pixel_buffer_format format);

    bool pixel_buffer_format_is_nv12(pixel_buffer_format format);

    bool pixel_buffer_format_is_i420(pixel_buffer_format format);

    bool pixel_buffer_format_is_yuv(pixel_buffer_format format);

    std::string pixel_buffer_format_to_str(pixel_buffer_format format);

    pixel_buffer_format yuv_format_to_pixel_buffer_format(const bnb::yuv_format_t& yuv_format);

    bnb::yuv_format_t pixel_buffer_format_to_yuv_format(pixel_buffer_format format);

    pixel_buffer_format pixel_format_to_pixel_buffer_format(bnb::interfaces::pixel_format format);

    bnb::interfaces::pixel_format pixel_buffer_format_to_pixel_format(pixel_buffer_format format);

} // namespace bnb::player_api
