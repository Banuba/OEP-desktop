#pragma once

#include <bnb/player_api/input/input_base.hpp>

namespace bnb::player_api
{

    class live_input : public bnb::player_api::input_base
    {
    public:
        live_input();

        ~live_input() = default;

        void push(const frame_data_sptr& fd);

        void push(const full_image_t& image, uint64_t timestamp_us = 0);

        void push(const pixel_buffer_sptr& image, uint64_t timestamp_us = 0);
    }; /* class stream_input */

} /* namespace bnb::player_api */
