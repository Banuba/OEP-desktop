#pragma once

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
        bpc8_rgb = 0x0101,  // one plane: RGB
        bpc8_bgr = 0x0102,  // one plane: BGR
        bpc8_rgba = 0x0103, // one plane: RGBA
        bpc8_bgra = 0x0104, // one plane: BGRA
        bpc8_argb = 0x0105, // one plane: ARGB
        nv12 = 0x0201,      // two planes: first RED, second RG
        i420 = 0x0401       // three planes: first RED, second RED, third RED
    }; // enum class pixel_buffer_format
    // clang-format on

    inline bool pixel_buffer_format_is_bpc8(pixel_buffer_format format)
    {
        return static_cast<bool>(static_cast<int32_t>(format) & 0x0100);
    }

    inline bool pixel_buffer_format_is_nv12(pixel_buffer_format format)
    {
        return static_cast<bool>(static_cast<int32_t>(format) & 0x0200);
    }

    inline bool pixel_buffer_format_is_i420(pixel_buffer_format format)
    {
        return static_cast<bool>(static_cast<int32_t>(format) & 0x0400);
    }

    inline bool pixel_buffer_format_is_yuv(pixel_buffer_format format)
    {
        return static_cast<bool>(static_cast<int32_t>(format) & 0x0600);
    }

    inline std::string_view pixel_buffer_format_to_str(pixel_buffer_format format)
    {
        switch (format) {
            case pixel_buffer_format::bpc8_rgb:
                return "bpc8_rgb";
            case pixel_buffer_format::bpc8_bgr:
                return "bpc8_bgr";
            case pixel_buffer_format::bpc8_rgba:
                return "bpc8_rgba";
            case pixel_buffer_format::bpc8_bgra:
                return "bpc8_bgra";
            case pixel_buffer_format::bpc8_argb:
                return "bpc8_argb";
            case pixel_buffer_format::nv12:
                return "nv12";
            case pixel_buffer_format::i420:
                return "i420";
        }
    }

} // namespace bnb::player_api
