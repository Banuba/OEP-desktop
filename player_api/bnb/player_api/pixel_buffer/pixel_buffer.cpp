#include <bnb/player_api/pixel_buffer/pixel_buffer.hpp>

namespace
{
    struct image_format_info
    {
        int32_t planes_num{0}; /* [1..3] - number of planes */
        int32_t pixel_sizes[3]{0, 0, 0};
    }; /* image_format_info */

    
    image_format_info get_image_format_info(bnb::player_api::pixel_buffer_format fmt)
    {
        image_format_info ret;
        using ns = bnb::player_api::pixel_buffer_format;
        switch (fmt) {
            case ns::bpc8_rgb:
            case ns::bpc8_bgr:
                return {1, 3, 0, 0};
            case ns::bpc8_rgba:
            case ns::bpc8_bgra:
            case ns::bpc8_argb:
                return {1, 4, 0, 0};
            case ns::nv12_bt601_full:
            case ns::nv12_bt601_video:
            case ns::nv12_bt709_full:
            case ns::nv12_bt709_video:
                return {2, 1, 2, 0};
            case ns::i420_bt601_full:
            case ns::i420_bt601_video:
            case ns::i420_bt709_full:
            case ns::i420_bt709_video:
                return {3, 1, 1, 1};
        }
        return {0, 0, 0, 0};;
    }
}

namespace bnb::player_api
{

    /* interfaces::pixel_buffer::create */
//    pixel_buffer_sptr bnb::oep::interfaces::pixel_buffer::create(const std::vector<plane_data>& planes, image_format fmt, int32_t width, int32_t height, std::function<void(bnb::oep::interfaces::pixel_buffer*)> deleter)
//    {
//        return pixel_buffer_sptr(new bnb::oep::pixel_buffer(planes, fmt, width, height), deleter);
//    }

    /* pixel_buffer::pixel_buffer */
    pixel_buffer::pixel_buffer(const std::vector<plane_data>& planes, pixel_buffer_format fmt, int32_t width, int32_t height)
    {
        auto info = get_image_format_info(fmt);
        if (info.planes_num != planes.size()) {
            throw std::runtime_error("[ERROR] The number of planes is strictly defined for: bpc8 - 1, nv12 - 2, i420 - 3");
        }

        static const int32_t plane_size_divider[3]{1, 2, 2};
        m_pixel_buffer_format = fmt;
        m_plane_count = info.planes_num;
        
        for (int i = 0; i < m_plane_count; i++) {
            m_planes[i].data = planes[i].data;
            m_planes[i].size = planes[i].size;
            m_planes[i].bytes_per_row = planes[i].bytes_per_row;
            m_planes[i].width = width / plane_size_divider[i];
            m_planes[i].height = height / plane_size_divider[i];
            m_planes[i].pixel_size = info.pixel_sizes[i];
        }
    }

    /* pixel_buffer::~pixel_buffer */
    pixel_buffer::~pixel_buffer()
    {
    }

    /* pixel_buffer::get_image_format */
    pixel_buffer_format pixel_buffer::get_format() const noexcept
    {
        return m_pixel_buffer_format;
    }

    /* pixel_buffer::get_number_of_planes */
    int32_t pixel_buffer::get_number_of_planes() const noexcept
    {
        return m_plane_count;
    }

    /* pixel_buffer::get_base_sptr */
    bnb::player_api::interfaces::pixel_buffer::plane_sptr pixel_buffer::get_base_sptr() const
    {
        return get_base_sptr_of_plane(0);
    }

    /* pixel_buffer::get_base_sptr_of_plane */
    bnb::player_api::interfaces::pixel_buffer::plane_sptr pixel_buffer::get_base_sptr_of_plane(int32_t plane_num) const
    {
        validate_plane_number(plane_num);
        return m_planes[plane_num].data;
    }

    /* pixel_buffer::get_bytes_per_pixel */
    int32_t pixel_buffer::get_bytes_per_pixel() const
    {
        return get_bytes_per_pixel_of_plane(0);
    }

    /* pixel_buffer::get_bytes_per_pixel_of_plane */
    int32_t pixel_buffer::get_bytes_per_pixel_of_plane(int32_t plane_num) const
    {
        validate_plane_number(plane_num);
        return m_planes[plane_num].pixel_size;
    }

    /* pixel_buffer::get_bytes_per_row */
    int32_t pixel_buffer::get_bytes_per_row() const
    {
        return get_bytes_per_row_of_plane(0);
    }

    /* pixel_buffer::get_bytes_per_row_of_plane */
    int32_t pixel_buffer::get_bytes_per_row_of_plane(int32_t plane_num) const
    {
        validate_plane_number(plane_num);
        return m_planes[plane_num].bytes_per_row;
    }

    /* pixel_buffer::get_width */
    int32_t pixel_buffer::get_width() const
    {
        return get_width_of_plane(0);
    }

    /* pixel_buffer::get_width_of_plane */
    int32_t pixel_buffer::get_width_of_plane(int32_t plane_num) const
    {
        validate_plane_number(plane_num);
        return m_planes[plane_num].width;
    }

    /* pixel_buffer::get_height */
    int32_t pixel_buffer::get_height() const
    {
        return get_height_of_plane(0);
    }

    /* pixel_buffer::get_height_of_plane */
    int32_t pixel_buffer::get_height_of_plane(int32_t plane_num) const
    {
        validate_plane_number(plane_num);
        return m_planes[plane_num].height;
    }

    /* pixel_buffer::validate_plane_number */
    void pixel_buffer::validate_plane_number(int32_t plane_num) const
    {
        if (plane_num < 0 || plane_num >= m_plane_count) {
            throw std::runtime_error("[ERROR] Invalid plane number. 'plane_num' must be in range [0..2].");
        }
    }

} /* namespace bnb::player_api */
