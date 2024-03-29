#include <bnb/player_api/interfaces/pixel_buffer/pixel_buffer.hpp>

#include <bnb/player_api/utils/yuv_conversion.hpp>

namespace
{

    using namespace bnb::player_api;

    class pixel_buffer_impl : public pixel_buffer
    {
    public:
        pixel_buffer_impl(
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

        pixel_buffer_impl(
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

        pixel_buffer_impl(
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

        ~pixel_buffer_impl() override
        {
            for (auto& plane : m_planes) {
                if (plane.deleter != nullptr) {
                    plane.deleter(plane.data);
                }
            }
        }

        orientation get_orientation() const noexcept override
        {
            return m_orientation;
        }

        bool get_mirroring() const noexcept override
        {
            return m_mirroring;
        }

        pixel_buffer_format get_format() const noexcept override
        {
            return m_pixel_buffer_format;
        }

        uint32_t get_number_of_planes() const noexcept override
        {
            return m_plane_count;
        }

        uint8_t* get_base_ptr() const override
        {
            return get_base_ptr_of_plane(0);
        }

        uint8_t* get_base_ptr_of_plane(uint32_t plane_num) const override
        {
            validate_plane_number(plane_num);
            return m_planes[plane_num].data;
        }

        int32_t get_bytes_per_pixel() const override
        {
            return get_bytes_per_pixel_of_plane(0);
        }

        int32_t get_bytes_per_pixel_of_plane(uint32_t plane_num) const override
        {
            validate_plane_number(plane_num);
            return m_planes[plane_num].pixel_size;
        }

        int32_t get_bytes_per_row() const override
        {
            return get_bytes_per_row_of_plane(0);
        }

        int32_t get_bytes_per_row_of_plane(uint32_t plane_num) const override
        {
            validate_plane_number(plane_num);
            return m_planes[plane_num].bytes_per_row;
        }

        int32_t get_width() const override
        {
            return get_width_of_plane(0);
        }

        int32_t get_width_of_plane(uint32_t plane_num) const override
        {
            validate_plane_number(plane_num);
            return m_planes[plane_num].width;
        }

        int32_t get_height() const override
        {
            return get_height_of_plane(0);
        }

        int32_t get_height_of_plane(uint32_t plane_num) const override
        {
            validate_plane_number(plane_num);
            return m_planes[plane_num].height;
        }

    private:
        struct plane_data
        {
            uint8_t* data{0};
            int32_t bytes_per_row{0};
            int32_t width{0};
            int32_t height{0};
            int32_t pixel_size{0};
            plane_deleter deleter{nullptr};
        }; /* struct plane_data_extended */

        void set_plane_data(uint32_t plane_num, const uint8_t* data, int32_t stride, int32_t pixel_size, int32_t width, int32_t height, plane_deleter deleter)
        {
            validate_plane_number(plane_num);
            m_planes[plane_num].data = const_cast<uint8_t*>(data);
            m_planes[plane_num].bytes_per_row = stride;
            m_planes[plane_num].width = width;
            m_planes[plane_num].height = height;
            m_planes[plane_num].pixel_size = pixel_size;
            m_planes[plane_num].deleter = deleter;
        }

        void validate_plane_number(uint32_t plane_num) const
        {
            if (plane_num >= m_plane_count) {
                throw std::runtime_error("Invalid plane number. 'plane_num' must be in range [0..2].");
            }
        }

    private:
        plane_data m_planes[3];
        pixel_buffer_format m_pixel_buffer_format;
        uint32_t m_plane_count{0};
        orientation m_orientation;
        bool m_mirroring;
    }; // class pixel_buffer_impl

} // namespace

namespace bnb::player_api
{

    std::shared_ptr<pixel_buffer> pixel_buffer::create(
        const uint8_t* rgb_plane,
        int32_t rgb_stride,
        int32_t width,
        int32_t height,
        pixel_buffer_format fmt,
        orientation orient,
        bool mirroring,
        plane_deleter deleter
    )
    {
        return std::make_shared<pixel_buffer_impl>(
            rgb_plane,
            rgb_stride,
            width,
            height,
            fmt,
            orient,
            mirroring,
            deleter
        );
    }

    std::shared_ptr<pixel_buffer> pixel_buffer::create(
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
    {
        return std::make_shared<pixel_buffer_impl>(
            y_plane,
            y_stride,
            uv_plane,
            uv_stride,
            width,
            height,
            fmt,
            orient,
            mirroring,
            y_deleter,
            uv_deleter
        );
    }

    std::shared_ptr<pixel_buffer> pixel_buffer::create(
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
    {
        return std::make_shared<pixel_buffer_impl>(
            y_plane,
            y_stride,
            u_plane,
            u_stride,
            v_plane,
            v_stride,
            width,
            height,
            fmt,
            orient,
            mirroring,
            y_deleter,
            u_deleter,
            v_deleter
        );
    }

} // namespace bnb::player_api
