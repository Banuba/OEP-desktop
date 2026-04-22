#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>
#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/player_api/interfaces/output/orientable.hpp>

#include <functional>

namespace bnb::player_api
{

    class BNB_EXPORT opengl_frame_output
        : public interfaces::output
        , public orientable
    {
    public:
        using pixel_buffer_callback = std::function<void(const pixel_buffer_sptr& pb)>;

        static std::shared_ptr<opengl_frame_output> create(const pixel_buffer_callback& callback, pixel_buffer_format format, bool start_receiving_frames = true);

        virtual void set_yuv_format_params(bnb::color_std std, bnb::color_range rng) = 0;

        /**
         * Process one frame and pause.
         */
        virtual void once() = 0;

        /**
         * Process frames as it arrives.
         */
        virtual void loop() = 0;
    }; // class opengl_frame_output

} // namespace bnb::player_api
