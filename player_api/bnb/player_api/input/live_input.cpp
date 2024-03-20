#include <bnb/player_api/input/live_input.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace bnb::player_api
{

    /* live_input::live_input */
    live_input::live_input()
    {
        auto config = bnb::interfaces::processor_configuration::create();
        config->set_use_offline_mode(false);
        config->set_use_future_interpolate(false);
        config->set_use_future_filter(false);
        auto processor_mode = bnb::interfaces::realtime_processor_mode::async_when_effect_loaded;
        m_frame_processor = bnb::interfaces::frame_processor::create_realtime_processor(processor_mode, config);
    }

    /* live_input::push */
    void live_input::push(const frame_data_sptr& fd)
    {
        push_frame_data(fd);
    }

    /* live_input::push */
    void live_input::push(const full_image_t& image, uint64_t timestamp_us)
    {
        push_full_image(image, timestamp_us);
    }

    /* live_input::push */
    void live_input::push(const pixel_buffer_sptr& image, uint64_t timestamp_us)
    {
        push_pixel_buffer(image, timestamp_us);
    }

} // namespace bnb::player_api
