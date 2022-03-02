#pragma once

#include <interfaces/pixel_buffer.hpp>
#include <bnb/spal/camera/base.hpp>

namespace bnb
{

    class camera_utils {
    public:
        camera_utils() = delete;

        static pixel_buffer_sptr full_image_to_pixel_buffer(bnb::full_image_t &image);
    };

} /* namespace bnb */
