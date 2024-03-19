#include <bnb/player_api/input/photo_input.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace bnb::player_api
{

    /* photo_input::photo_input */
    photo_input::photo_input()
    {
        auto config = bnb::interfaces::processor_configuration::create();
        config->set_use_offline_mode(true);
        config->set_use_future_interpolate(false);
        config->set_use_future_filter(false);
        m_frame_processor = bnb::interfaces::frame_processor::create_photo_processor(config);
    }

    /* photo_input::push */
    void photo_input::push(const frame_data_sptr& fd)
    {
        push_frame_data(fd);
    }

    /* photo_input::push */
    void photo_input::push(const full_image_t& image, uint64_t timestamp_us)
    {
        push_full_image(image, timestamp_us);
    }

    /* photo_input::push */
    void photo_input::push(const pixel_buffer_sptr& image, uint64_t timestamp_us)
    {
        push_pixel_buffer(image, timestamp_us);
    }

} /* namespace bnb::player_api */
