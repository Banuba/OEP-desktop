#pragma once

#include <bnb/player_api/interfaces/input.hpp>
#include <bnb/player_api/interfaces/pixel_buffer.hpp>

namespace bnb::player_api
{

    class input_base : public bnb::player_api::interfaces::input
    {
    public:
        frame_processor_sptr get_frame_processor() const noexcept override;

        uint64_t get_frame_time_us() const noexcept override;

    protected:
        void push_frame_data(const frame_data_sptr& fd);

        void push_full_image(const full_image_t& image, uint64_t timestamp_us);

        void push_pixel_buffer(const pixel_buffer_sptr& image, uint64_t timestamp_us);

    protected:
        frame_processor_sptr m_frame_processor {nullptr};
        uint64_t m_timestamp {0};
    }; /* class input_base */

} /* namespace bnb::player_api */
