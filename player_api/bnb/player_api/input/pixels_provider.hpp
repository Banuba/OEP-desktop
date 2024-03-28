#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>

namespace bnb::player_api
{

    class pixels_provider
    {
    public:
        virtual void push(const frame_data_sptr& fd) = 0;

        virtual void push(const full_image_t& image, uint64_t timestamp_us = 0) = 0;

        virtual void push(const pixel_buffer_sptr& image, uint64_t timestamp_us = 0) = 0;
    }; // class pixels_provider

} // namespace bnb::player_api
