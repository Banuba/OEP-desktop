#pragma once

#include <bnb/player_api/interfaces/input.hpp>
#include <bnb/player_api/interfaces/pixel_buffer.hpp>
#include <bnb/player_api/types/orientation.hpp>

namespace bnb::player_api
{

    class stream_input : public bnb::player_api::interfaces::input
    {
    public:
        stream_input();

        ~stream_input() = default;

        void push(const frame_data_sptr& fd);
        
        void push(const full_image_t& image, uint64_t timestamp_us = 0);
        
        void push(const pixel_buffer_sptr& image, uint64_t timestamp_us = 0, orientation orient = orientation::up, bool require_mirroring = false);

        frame_processor_sptr get_frame_processor() const noexcept override;

        uint64_t get_frame_time_us() const noexcept override;

    private:
        frame_processor_sptr m_frame_processor;
        uint64_t m_timestamp = 0;
    }; /* class stream_input */

} /* namespace bnb::player_api */
