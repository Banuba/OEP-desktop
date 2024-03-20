#include <bnb/player_api/input/stream_input.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace bnb::player_api
{

    /* stream_input::stream_input */
    stream_input::stream_input()
    {
        auto config = bnb::interfaces::processor_configuration::create();
        config->set_use_offline_mode(false);
        config->set_use_future_interpolate(false);
        config->set_use_future_filter(false);
        m_frame_processor = bnb::interfaces::frame_processor::create_video_processor(config);
    }

    /* stream_input::push */
    void stream_input::push(const frame_data_sptr& fd)
    {
        push_frame_data(fd);
    }

    /* stream_input::push */
    void stream_input::push(const full_image_t& image, uint64_t timestamp_us)
    {
        push_full_image(image, timestamp_us);
    }

    /* stream_input::push */
    void stream_input::push(const pixel_buffer_sptr& image, uint64_t timestamp_us)
    {
        push_pixel_buffer(image, timestamp_us);
    }

} // namespace bnb::player_api
