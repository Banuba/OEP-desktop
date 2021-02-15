#pragma once

#include <bnb/types/full_image.hpp>

using oep_image_ready_cb = std::function<void(bnb::full_image_t image)>;

namespace bnb::interfaces
{
    class pixel_buffer
    {
    public:
        virtual ~pixel_buffer() = default;

        /**
         * In thread with active texture get pixel bytes from Offscreen_render_target and
         * convert to full_image_t.
         * 
         * @param callback calling with full_image_t. full_image_t keep RGBA
         * 
         * Example process_image_async([](full_image_t image){})
         */
        virtual void get_rgba(oep_image_ready_cb callback) = 0;

        /**
         * In thread with active texture get pixel bytes from Offscreen_render_target and
         * convert to full_image_t.
         * 
         * @param callback calling with full_image_t. full_image_t keep NV12
         * 
         * Example process_image_async([](full_image_t image){})
         */
        virtual void get_nv12(oep_image_ready_cb callback) = 0;
    };
} // bnb::interfaces

