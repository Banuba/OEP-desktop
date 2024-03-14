#pragma once

#include <bnb/player_api/types/pixel_buffer_format.hpp>

#include <memory>

namespace bnb::player_api::interfaces
{
    class pixel_buffer;
} /* namespace bnb::player_api::interfaces */

namespace bnb::player_api
{
    using pixel_buffer_sptr = std::shared_ptr<bnb::player_api::interfaces::pixel_buffer>;
} /* namespace bnb::player_api */

namespace bnb::player_api::interfaces
{

    class pixel_buffer
    {
    public:
        virtual ~pixel_buffer() = default;

        /**
         * Returns format of the current image
         * @return fortmat of the pixel_buffer
         */
        virtual pixel_buffer_format get_format() const noexcept = 0;

        /**
         * Returns count of the planes
         */
        virtual int32_t get_number_of_planes() const noexcept = 0;

        /**
         * Returns the shared pointer of the first plane
         */
        virtual uint8_t* get_base_ptr() const = 0;

        /**
         * Returns the shared pointer to pixel data of the specified plane
         * @param plane_num plane number. Must be 0 for bpc8, [0..1] for nv12 and [0..2] for i420 images
         */
        virtual uint8_t* get_base_ptr_of_plane(int32_t plane_num) const = 0;

        /**
         * Returns the pixel size of the first plane
         */
        virtual int32_t get_bytes_per_pixel() const = 0;

        /**
         * Returns the pixel size of the specified plane
         * @param plane_num plane number. Must be 0 for bpc8, [0..1] for nv12 and [0..2] for i420 images
         */
        virtual int32_t get_bytes_per_pixel_of_plane(int32_t plane_num) const = 0;

        /**
         * Returns the stride of the first plane
         */
        virtual int32_t get_bytes_per_row() const = 0;

        /**
         * Returns the stride of the specified plane
         * @param plane_num plane number. Must be 0 for bpc8, [0..1] for nv12 and [0..2] for i420 images
         */
        virtual int32_t get_bytes_per_row_of_plane(int32_t plane_num) const = 0;

        /**
         * Returns the width of the first plane
         */
        virtual int32_t get_width() const = 0;

        /**
         * Returns the width of the specified plane
         * @param plane_num plane number. Must be 0 for bpc8, [0..1] for nv12 and [0..2] for i420 images
         */
        virtual int32_t get_width_of_plane(int32_t plane_num) const = 0;

        /**
         * Returns the height of the first plane
         */
        virtual int32_t get_height() const = 0;

        /**
         * Returns the height of the specified plane
         * @param plane_num plane number. Must be 0 for bpc8, [0..1] for nv12 and [0..2] for i420 images
         */
        virtual int32_t get_height_of_plane(int32_t plane_num) const = 0;
    }; /* class pixel_buffer */

} /* namespace bnb::player_api::interfaces */
