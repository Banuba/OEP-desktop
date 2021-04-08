#pragma once

#include <bnb/types/full_image.hpp>

using oep_image_ready_cb = std::function<void(std::optional<bnb::full_image_t> image)>;

namespace bnb::interfaces
{
    class pixel_buffer
    {
    public:
        virtual ~pixel_buffer() = default;

        /**
         * Lock pixel buffer. If you want to keep lock of pixel buffer
         * longer than output image callback scope you should lock pixel buffer.
         * 
         * Example lock()
         */
        virtual void lock() = 0;

        /**
         * Unlock pixel_buffer. Must be called if user explicitly called lock()
         * after the work to process output pixel buffer completed.
         * 
         * Example unlock()
         */
        virtual void unlock() = 0;

        /**
         * Returns the locking state of pixel_buffer.
         * 
         * @return true if pixel_buffer locked else false
         * 
         * Example is_locked()
         */
        virtual bool is_locked() = 0;

        /**
         * In thread with active texture get pixel bytes from Offscreen_render_target and
         * convert to full_image_t.
         * 
         * @param callback calling with full_image_t. full_image_t keep RGBA
         * 
         * Example process_image_async([](std::optional<full_image_t> image){})
         */
        virtual void get_rgba(oep_image_ready_cb callback) = 0;

        /**
         * In thread with active texture get pixel bytes from Offscreen_render_target and
         * convert to full_image_t.
         * 
         * @param callback calling with full_image_t. full_image_t keep NV12
         * 
         * Example process_image_async([](std::optional<full_image_t> image){})
         */
        virtual void get_nv12(oep_image_ready_cb callback) = 0;
    };
} // bnb::interfaces

using ipb_sptr = std::shared_ptr<bnb::interfaces::pixel_buffer>;