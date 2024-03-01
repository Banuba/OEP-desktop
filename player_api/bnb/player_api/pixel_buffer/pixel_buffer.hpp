#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>

namespace bnb::player_api
{

    class pixel_buffer : public bnb::player_api::interfaces::pixel_buffer
    {
    public:
        pixel_buffer(const std::vector<plane_data>& planes, pixel_buffer_format fmt, int32_t width, int32_t height);
        ~pixel_buffer();

        pixel_buffer_format get_format() const noexcept override;
        
        int32_t get_number_of_planes() const noexcept override;

        plane_sptr get_base_sptr() const override;
        plane_sptr get_base_sptr_of_plane(int32_t plane_num) const override;

        int32_t get_bytes_per_pixel() const override;
        int32_t get_bytes_per_pixel_of_plane(int32_t plane_num) const override;
        int32_t get_bytes_per_row() const override;
        int32_t get_bytes_per_row_of_plane(int32_t plane_num) const override;
        int32_t get_width() const override;
        int32_t get_width_of_plane(int32_t plane_num) const override;
        int32_t get_height() const override;
        int32_t get_height_of_plane(int32_t plane_num) const override;

    private:
        struct plane_data_extended : public plane_data
        {
            int32_t width{0};
            int32_t height{0};
            int32_t pixel_size{0};
        }; /* struct plane_data_extended */

        void validate_plane_number(int32_t plane_num) const;
        
    private:
        pixel_buffer_format m_pixel_buffer_format;
        int32_t m_plane_count{0};
        plane_data_extended m_planes[3];
    }; /* class image_data  */


} /* namespace bnb::player_api */
