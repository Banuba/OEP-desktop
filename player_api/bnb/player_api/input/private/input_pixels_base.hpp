#pragma once

#include <bnb/player_api/interfaces/input.hpp>
#include <bnb/player_api/input/pixels_provider.hpp>
#include <bnb/player_api/interfaces/pixel_buffer.hpp>

namespace bnb::player_api
{

    class input_pixels_base
        : public virtual bnb::player_api::interfaces::input
        , public virtual bnb::player_api::pixels_provider
    {
    public:
        frame_processor_sptr get_frame_processor() const noexcept override;

        uint64_t get_frame_time_us() const noexcept override;

        void push(const frame_data_sptr& fd) override;

        void push(const full_image_t& image, uint64_t timestamp_us = 0) override;

        void push(const pixel_buffer_sptr& image, uint64_t timestamp_us = 0) override;

    protected:
        frame_processor_sptr m_frame_processor{nullptr};
        uint64_t m_timestamp{0};
    }; // class input_pixels_base

} // namespace bnb::player_api
