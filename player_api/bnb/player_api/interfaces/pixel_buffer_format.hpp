#pragma once

#include <cstdint>

namespace bnb::player_api
{

    /* bt601 and bt709 - two standards for representing color space that use the same
     * image encoding/decoding algorithm. Differ in the encoding / decoding coefficients.
     */
    enum class pixel_buffer_format : int32_t
    {
        bpc8_rgb,         /* one plane: RGB */
        bpc8_bgr,         /* one plane: BGR */
        bpc8_rgba,        /* one plane: RGBA */
        bpc8_bgra,        /* one plane: BGRA */
        bpc8_argb,        /* one plane: ARGB */
        nv12_bt601_full,  /* two planes: first RED, second RG */
        nv12_bt601_video, /* two planes: first RED, second RG */
        nv12_bt709_full,  /* two planes: first RED, second RG */
        nv12_bt709_video, /* two planes: first RED, second RG */
        i420_bt601_full,  /* three planes: first RED, second RED, third RED */
        i420_bt601_video, /* three planes: first RED, second RED, third RED */
        i420_bt709_full,  /* three planes: first RED, second RED, third RED */
        i420_bt709_video  /* three planes: first RED, second RED, third RED */
    }; /* enum class image_format */

} /* namespace bnb::player_api */
