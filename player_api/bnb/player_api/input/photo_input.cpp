#include <bnb/player_api/interfaces/input/photo_input.hpp>

#include <bnb/player_api/input/input_pixels_base.hpp>
#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace
{

    using namespace bnb::player_api;

    class photo_input_impl
        : public virtual photo_input
        , public virtual input_pixels_base
    {
    public:
        photo_input_impl()
        {
            auto config = bnb::interfaces::processor_configuration::create();
            config->set_use_offline_mode(true);
            config->set_use_future_interpolate(false);
            config->set_use_future_filter(false);
            m_frame_processor = bnb::interfaces::frame_processor::create_photo_processor(config);
        }

        ~photo_input_impl() = default;

        void load(const std::string& path) override
        {
            auto image = bnb::full_image_t::load(path);
            push(std::move(image));
        }
    }; // class photo_input_impl

} // namespace

std::shared_ptr<bnb::player_api::photo_input> bnb::player_api::photo_input::create()
{
    return std::make_shared<photo_input_impl>();
}
