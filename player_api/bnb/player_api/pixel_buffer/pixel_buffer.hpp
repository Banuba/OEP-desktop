#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>
#include <functional>

namespace bnb::player_api
{

    class pixel_buffer : public bnb::player_api::interfaces::pixel_buffer
    {
    public:
        using plane_deleter = std::function<void(uint8_t*)>;

    public:
        pixel_buffer(
            const uint8_t* rgb_plane,
            int32_t rgb_stride,
            pixel_buffer_format fmt,
            int32_t width,
            int32_t height,
            plane_deleter deleter);

        pixel_buffer(
            const uint8_t* y_plane,
            int32_t y_stride,
            const uint8_t* uv_plane,
            int32_t uv_stride,
            pixel_buffer_format fmt,
            int32_t width,
            int32_t height,
            plane_deleter y_deleter,
            plane_deleter uv_deleter);

        pixel_buffer(
            const uint8_t* y_plane,
            int32_t y_stride,
            const uint8_t* u_plane,
            int32_t u_stride,
            const uint8_t* v_plane,
            int32_t v_stride,
            pixel_buffer_format fmt,
            int32_t width,
            int32_t height,
            plane_deleter y_deleter,
            plane_deleter u_deleter,
            plane_deleter v_deleter);

        ~pixel_buffer();

        pixel_buffer_format get_format() const noexcept override;
        
        int32_t get_number_of_planes() const noexcept override;

        uint8_t* get_base_ptr() const override;
        uint8_t* get_base_ptr_of_plane(int32_t plane_num) const override;

        int32_t get_bytes_per_pixel() const override;
        int32_t get_bytes_per_pixel_of_plane(int32_t plane_num) const override;
        int32_t get_bytes_per_row() const override;
        int32_t get_bytes_per_row_of_plane(int32_t plane_num) const override;
        int32_t get_width() const override;
        int32_t get_width_of_plane(int32_t plane_num) const override;
        int32_t get_height() const override;
        int32_t get_height_of_plane(int32_t plane_num) const override;

    private:
        struct plane_data
        {
            uint8_t* data {0};
            int32_t bytes_per_row {0};
            int32_t width {0};
            int32_t height {0};
            int32_t pixel_size {0};
            plane_deleter deleter {nullptr};
        }; /* struct plane_data_extended */

        void validate_plane_number(int32_t plane_num) const;
        
    private:
        pixel_buffer_format m_pixel_buffer_format;
        int32_t m_plane_count{0};
        plane_data m_planes[3];
    }; /* class image_data  */


} /* namespace bnb::player_api */
