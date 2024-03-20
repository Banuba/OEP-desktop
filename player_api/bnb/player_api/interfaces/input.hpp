#pragma once

#include <bnb/effect_player/interfaces/frame_processor.hpp>
#include <bnb/types/interfaces/frame_data.hpp>
#include <bnb/types/full_image.hpp>

namespace bnb::player_api::interfaces
{
    class input;
} // namespace bnb::player_api::interfaces

namespace bnb::player_api
{
    using frame_data_sptr = std::shared_ptr<bnb::interfaces::frame_data>;
    using full_image_sptr = std::shared_ptr<bnb::full_image_t>;
    using input_sptr = std::shared_ptr<bnb::player_api::interfaces::input>;
    using frame_processor_sptr = std::shared_ptr<bnb::interfaces::frame_processor>;
} // namespace bnb::player_api

namespace bnb::player_api::interfaces
{

    /**
     * The interface is inherited by all classes that must work with the player,
     * responsible for providing frames from outside in the player.
     */
    class input
    {
    public:
        virtual ~input() = default;

        /**
         * Get last available frame
         * @return frame_processor with filled frame_data
         */
        virtual frame_processor_sptr get_frame_processor() const noexcept = 0;

        /**
         * Get timestamp of the current frame.
         * @return timestamp
         */
        virtual uint64_t get_frame_time_us() const noexcept = 0;
    }; // class input

} // namespace bnb::player_api::interfaces
