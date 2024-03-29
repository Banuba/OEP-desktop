#pragma once

#include <bnb/utils/defs.hpp>

#include <cstdint>
#include <string>

namespace bnb::player_api
{

    /**
     * bt601 and bt709 - two standards for representing color space that use the same
     * image encoding/decoding algorithm. Differ in the encoding / decoding coefficients.
     */
    // clang-format off
    enum class pixel_buffer_format : int32_t
    {
        bpc8_rgb,  // one plane: RGB
        bpc8_bgr,  // one plane: BGR
        bpc8_rgba, // one plane: RGBA
        bpc8_bgra, // one plane: BGRA
        bpc8_argb, // one plane: ARGB
        nv12,      // two planes: first RED, second RG
        i420       // three planes: first RED, second RED, third RED
    }; // enum class pixel_buffer_format
    // clang-format on

    BNB_EXPORT bool pixel_buffer_format_is_bpc8(pixel_buffer_format format);

    BNB_EXPORT bool pixel_buffer_format_is_nv12(pixel_buffer_format format);

    BNB_EXPORT bool pixel_buffer_format_is_i420(pixel_buffer_format format);

    BNB_EXPORT bool pixel_buffer_format_is_yuv(pixel_buffer_format format);

    BNB_EXPORT std::string pixel_buffer_format_to_str(pixel_buffer_format format);

} // namespace bnb::player_api
