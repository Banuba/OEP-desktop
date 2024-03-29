#include <bnb/player_api/interfaces/types/pixel_buffer_format.hpp>

#include <bnb/types/full_image.hpp>

namespace bnb::player_api
{

    bool pixel_buffer_format_is_bpc8(pixel_buffer_format f)
    {
        using t = pixel_buffer_format;
        return f == t::bpc8_rgb || f == t::bpc8_bgr || f == t::bpc8_rgba || f == t::bpc8_bgra || f == t::bpc8_argb;
    }

    bool pixel_buffer_format_is_nv12(pixel_buffer_format f)
    {
        return f == pixel_buffer_format::nv12;
    }

    bool pixel_buffer_format_is_i420(pixel_buffer_format f)
    {
        return f == pixel_buffer_format::i420;
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
            case t::nv12:
                return "nv12";
            case t::i420:
                return "i420";
        }
    }

} // namespace bnb::player_api
