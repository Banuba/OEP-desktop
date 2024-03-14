#pragma once

#include <cstdint>
#include <string>

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
    }; /* enum class pixel_buffer_format */



    inline bool pixel_buffer_format_is_bpc8(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        return f == t::bpc8_rgb || f == t::bpc8_bgr || f == t::bpc8_rgba || f == t::bpc8_bgra || f == t::bpc8_argb;
    }

    inline bool pixel_buffer_format_is_nv12(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        return f == t::nv12_bt601_full || f == t::nv12_bt601_video || f == t::nv12_bt709_full || f == t::nv12_bt709_video;
    }

    inline bool pixel_buffer_format_is_i420(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        return f == t::i420_bt601_full || f == t::i420_bt601_video || f == t::i420_bt709_full || f == t::i420_bt709_video;
    }

    inline std::string pixel_buffer_format_to_str(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        switch (f) {
            case t::bpc8_rgb:
                return "bpc8_rgb";
            case t::bpc8_bgr:
                return "bpc8_bgr";
            case t::bpc8_rgba:
                return "bpc8_rgba";
            case t::bpc8_bgra:
                return "bpc8_bgra";
            case t::bpc8_argb:
                return "bpc8_argb";
            case t::nv12_bt601_full:
                return "nv12_bt601_full";
            case t::nv12_bt601_video:
                return "nv12_bt601_video";
            case t::nv12_bt709_full:
                return "nv12_bt709_full";
            case t::nv12_bt709_video:
                return "nv12_bt709_video";
            case t::i420_bt601_full:
                return "i420_bt601_full";
            case t::i420_bt601_video:
                return "i420_bt601_video";
            case t::i420_bt709_full:
                return "i420_bt709_full";
            case t::i420_bt709_video:
                return "i420_bt709_video";
        }
    }

} /* namespace bnb::player_api */
