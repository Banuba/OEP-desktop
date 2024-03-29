#include <bnb/player_api/interfaces/input/live_input.hpp>

#include <bnb/player_api/input/input_pixels_base.hpp>
#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace
{

    using namespace bnb::player_api;

    class live_input_impl
        : public virtual live_input
        , public virtual input_pixels_base
    {
    public:
        live_input_impl()
        {
            auto config = bnb::interfaces::processor_configuration::create();
            config->set_use_offline_mode(false);
            config->set_use_future_interpolate(false);
            config->set_use_future_filter(true);
            auto processor_mode = bnb::interfaces::realtime_processor_mode::async_when_effect_loaded;
            m_frame_processor = bnb::interfaces::frame_processor::create_realtime_processor(processor_mode, config);
        }

        ~live_input_impl() = default;
    }; // class live_input_impl

} // namespace

std::shared_ptr<bnb::player_api::live_input> bnb::player_api::live_input::create()
{
    return std::make_shared<live_input_impl>();
}
