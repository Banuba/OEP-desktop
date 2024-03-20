#include <bnb/player_api/pixel_buffer/pixel_buffer.hpp>

#include <bnb/player_api/types/yuv_conversion.hpp>

namespace bnb::player_api
{

    /* pixel_buffer::pixel_buffer */
    pixel_buffer::pixel_buffer(
        const uint8_t* rgb_plane,
        int32_t rgb_stride,
        int32_t width,
        int32_t height,
        pixel_buffer_format fmt,
        orientation orient,
        bool mirroring,
        plane_deleter deleter
    )
        : m_pixel_buffer_format(fmt)
        , m_plane_count(1)
        , m_orientation(orient)
        , m_mirroring(mirroring)
    {
        if (!pixel_buffer_format_is_bpc8(fmt)) {
            throw std::runtime_error("Format should be bpc8.");
        }

        auto pixel_size = fmt == pixel_buffer_format::bpc8_bgr || fmt == pixel_buffer_format::bpc8_rgb ? 3 : 4;
        set_plane_data(0, rgb_plane, rgb_stride, pixel_size, width, height, deleter);
    }

    /* pixel_buffer::pixel_buffer */
    pixel_buffer::pixel_buffer(
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
    )
        : m_pixel_buffer_format(fmt)
        , m_plane_count(2)
        , m_orientation(orient)
        , m_mirroring(mirroring)
    {
        if (!pixel_buffer_format_is_nv12(fmt)) {
            throw std::runtime_error("Format should be nv12.");
        }

        set_plane_data(0, y_plane, y_stride, 1, width, height, y_deleter);
        set_plane_data(1, uv_plane, uv_stride, 2, uv_plane_width(width), uv_plane_height(height), uv_deleter);
    }

    pixel_buffer::pixel_buffer(
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
    )
        : m_pixel_buffer_format(fmt)
        , m_plane_count(3)
        , m_orientation(orient)
        , m_mirroring(mirroring)
    {
        if (!pixel_buffer_format_is_i420(fmt)) {
            throw std::runtime_error("Format should be i420.");
        }

        set_plane_data(0, y_plane, y_stride, 1, width, height, y_deleter);
        set_plane_data(1, u_plane, u_stride, 1, uv_plane_width(width), uv_plane_height(height), u_deleter);
        set_plane_data(2, v_plane, v_stride, 1, uv_plane_width(width), uv_plane_height(height), v_deleter);
    }

    /* pixel_buffer::~pixel_buffer */
    pixel_buffer::~pixel_buffer()
    {
        for (auto& plane : m_planes) {
            if (plane.deleter != nullptr) {
                plane.deleter(plane.data);
            }
        }
    }

    /* pixel_buffer::get_orientation */
    orientation pixel_buffer::get_orientation() const noexcept
    {
        return m_orientation;
    }

    /* pixel_buffer::get_mirroring */
    bool pixel_buffer::get_mirroring() const noexcept
    {
        return m_mirroring;
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

    /* pixel_buffer::get_base_ptr */
    uint8_t* pixel_buffer::get_base_ptr() const
    {
        return get_base_ptr_of_plane(0);
    }

    /* pixel_buffer::get_base_ptr_of_plane */
    uint8_t* pixel_buffer::get_base_ptr_of_plane(int32_t plane_num) const
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

    /* pixel_buffer::set_plane_data */
    void pixel_buffer::set_plane_data(int32_t plane_num, const uint8_t* data, int32_t stride, int32_t pixel_size, int32_t width, int32_t height, plane_deleter deleter)
    {
        validate_plane_number(plane_num);
        m_planes[plane_num].data = const_cast<uint8_t*>(data);
        m_planes[plane_num].bytes_per_row = stride;
        m_planes[plane_num].width = width;
        m_planes[plane_num].height = height;
        m_planes[plane_num].pixel_size = pixel_size;
        m_planes[plane_num].deleter = deleter;
    }

    /* pixel_buffer::validate_plane_number */
    void pixel_buffer::validate_plane_number(int32_t plane_num) const
    {
        if (plane_num < 0 || plane_num >= m_plane_count) {
            throw std::runtime_error("Invalid plane number. 'plane_num' must be in range [0..2].");
        }
    }

} // namespace bnb::player_api
