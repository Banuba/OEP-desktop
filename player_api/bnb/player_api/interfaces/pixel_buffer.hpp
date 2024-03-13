#pragma once

#include <bnb/player_api/types/pixel_buffer_format.hpp>

#include <memory>
#include <functional>

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
        using plane_sptr = std::shared_ptr<uint8_t>;

        struct plane_data
        {
            plane_sptr data{nullptr};
            size_t size{0};
            int32_t bytes_per_row{0};
        }; /* struct plane_data */

    public:
        /**
         * Create the pixel buffer and returns shared pointer.
         *
         * @param planes with pixel data
         * @param fmt input image format
         * @param width Inpuut width of the image
         * @param width Inpuut height of the image
         * @param deleter custom deleter for pixel buffer
         *
         * @example bnb::player_api::interfaces::pixel_buffer::create(planes, pixel_buffer_format::bpc8_rgba, 1280, 720)
         */
        static pixel_buffer_sptr create(const std::vector<plane_data>& planes, pixel_buffer_format fmt, int32_t width, int32_t height, std::function<void(pixel_buffer*)> deleter = std::default_delete<pixel_buffer>());

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
        virtual plane_sptr get_base_sptr() const = 0;

        /**
         * Returns the shared pointer to pixel data of the specified plane
         * @param plane_num plane number. Must be 0 for bpc8, [0..1] for nv12 and [0..2] for i420 images
         */
        virtual plane_sptr get_base_sptr_of_plane(int32_t plane_num) const = 0;

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
