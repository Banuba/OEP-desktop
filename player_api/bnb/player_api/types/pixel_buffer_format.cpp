#include <bnb/player_api/types/pixel_buffer_format.hpp>

#include <bnb/types/full_image.hpp>

namespace bnb::player_api
{

    bool pixel_buffer_format_is_bpc8(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        return f == t::bpc8_rgb || f == t::bpc8_bgr || f == t::bpc8_rgba || f == t::bpc8_bgra || f == t::bpc8_argb;
    }

    bool pixel_buffer_format_is_nv12(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        return f == t::nv12_bt601_full || f == t::nv12_bt601_video || f == t::nv12_bt709_full || f == t::nv12_bt709_video;
    }

    bool pixel_buffer_format_is_i420(pixel_buffer_format f)
    {
        using t = bnb::player_api::pixel_buffer_format;
        return f == t::i420_bt601_full || f == t::i420_bt601_video || f == t::i420_bt709_full || f == t::i420_bt709_video;
    }

    bool pixel_buffer_format_is_yuv(pixel_buffer_format format)
    {
        return pixel_buffer_format_is_nv12(format) || pixel_buffer_format_is_i420(format);
    }

    std::string pixel_buffer_format_to_str(pixel_buffer_format f)
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

    pixel_buffer_format yuv_format_to_pixel_buffer_format(const bnb::yuv_format_t& yuv_format)
    {
        if (yuv_format.format == bnb::yuv_format::yuv_nv12) {
            if (yuv_format.standard == bnb::color_std::bt601) {
                if (yuv_format.range == bnb::color_range::full) {
                    return pixel_buffer_format::nv12_bt601_full;
                } else { // bnb::color_range::video
                    return pixel_buffer_format::nv12_bt601_video;
                }
            } else { // bnb::color_std::bt709
                if (yuv_format.range == bnb::color_range::full) {
                    return pixel_buffer_format::nv12_bt709_full;
                } else { // bnb::color_range::video
                    return pixel_buffer_format::nv12_bt709_video;
                }
            }
        } else { // bnb::yuv_format::yuv_i420
            if (yuv_format.standard == bnb::color_std::bt601) {
                if (yuv_format.range == bnb::color_range::full) {
                    return pixel_buffer_format::i420_bt601_full;
                } else { // bnb::color_range::video
                    return pixel_buffer_format::i420_bt601_video;
                }
            } else { // bnb::color_std::bt709
                if (yuv_format.range == bnb::color_range::full) {
                    return pixel_buffer_format::i420_bt709_full;
                } else { // bnb::color_range::video
                    return pixel_buffer_format::i420_bt709_video;
                }
            }
        }
    }

    bnb::yuv_format_t pixel_buffer_format_to_yuv_format(pixel_buffer_format format)
    {
        switch (format) {
            case pixel_buffer_format::nv12_bt601_full:
                return {bnb::color_range::full, bnb::color_std::bt601, bnb::yuv_format::yuv_nv12};
            case pixel_buffer_format::nv12_bt601_video:
                return {bnb::color_range::video, bnb::color_std::bt601, bnb::yuv_format::yuv_nv12};
            case pixel_buffer_format::nv12_bt709_full:
                return {bnb::color_range::full, bnb::color_std::bt709, bnb::yuv_format::yuv_nv12};
            case pixel_buffer_format::nv12_bt709_video:
                return {bnb::color_range::video, bnb::color_std::bt709, bnb::yuv_format::yuv_nv12};
            case pixel_buffer_format::i420_bt601_full:
                return {bnb::color_range::full, bnb::color_std::bt601, bnb::yuv_format::yuv_i420};
            case pixel_buffer_format::i420_bt601_video:
                return {bnb::color_range::video, bnb::color_std::bt601, bnb::yuv_format::yuv_i420};
            case pixel_buffer_format::i420_bt709_full:
                return {bnb::color_range::full, bnb::color_std::bt709, bnb::yuv_format::yuv_i420};
            case pixel_buffer_format::i420_bt709_video:
                return {bnb::color_range::video, bnb::color_std::bt709, bnb::yuv_format::yuv_i420};
            default:
                throw std::runtime_error("Incorrect pixel buffer format, only nv12 and i420 formats available.");
        }
    }

    pixel_buffer_format pixel_format_to_pixel_buffer_format(bnb::interfaces::pixel_format format)
    {
        switch (format) {
            case bnb::interfaces::pixel_format::rgb:
                return pixel_buffer_format::bpc8_rgb;
            case bnb::interfaces::pixel_format::rgba:
                return pixel_buffer_format::bpc8_rgba;
            case bnb::interfaces::pixel_format::bgr:
                return pixel_buffer_format::bpc8_bgr;
            case bnb::interfaces::pixel_format::bgra:
                return pixel_buffer_format::bpc8_bgra;
            case bnb::interfaces::pixel_format::argb:
                return pixel_buffer_format::bpc8_argb;
            default:
                throw std::runtime_error("Unknown pixel format.");
        }
    }

    bnb::interfaces::pixel_format pixel_buffer_format_to_pixel_format(pixel_buffer_format format)
    {
        switch (format) {
            case pixel_buffer_format::bpc8_rgb:
                return bnb::interfaces::pixel_format::rgb;
            case pixel_buffer_format::bpc8_rgba:
                return bnb::interfaces::pixel_format::rgba;
            case pixel_buffer_format::bpc8_bgr:
                return bnb::interfaces::pixel_format::bgr;
            case pixel_buffer_format::bpc8_bgra:
                return bnb::interfaces::pixel_format::bgra;
            case pixel_buffer_format::bpc8_argb:
                return bnb::interfaces::pixel_format::argb;
            default:
                throw std::runtime_error("Incorrect pixel buffer format, only bpc8 format available.");
        }
    }

} // namespace bnb::player_api
