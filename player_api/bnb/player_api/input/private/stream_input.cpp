#include <bnb/player_api/input/stream_input.hpp>

#include <bnb/player_api/input/private/input_pixels_base.hpp>
#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace
{

    using namespace bnb::player_api;

    class stream_input_impl
        : public virtual stream_input
        , public virtual input_pixels_base
    {
    public:
        stream_input_impl()
        {
            auto config = bnb::interfaces::processor_configuration::create();
            config->set_use_offline_mode(false);
            config->set_use_future_interpolate(false);
            config->set_use_future_filter(false);
            m_frame_processor = bnb::interfaces::frame_processor::create_video_processor(config);
        }

        ~stream_input_impl() = default;
    }; // class stream_input_impl

} // namespace

std::shared_ptr<bnb::player_api::stream_input> bnb::player_api::stream_input::create()
{
    return std::make_shared<stream_input_impl>();
}
