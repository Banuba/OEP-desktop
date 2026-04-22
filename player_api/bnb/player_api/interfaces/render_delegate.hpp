#pragma once

#include <memory>

namespace bnb::player_api::interfaces
{
    class render_delegate;
} // namespace bnb::player_api::interfaces

namespace bnb::player_api
{
    using render_delegate_sptr = std::shared_ptr<bnb::player_api::interfaces::render_delegate>;
} // namespace bnb::player_api

namespace bnb::player_api::interfaces
{

    class render_delegate
    {
    public:
        virtual ~render_delegate() = default;

        /**
         * Activate rendering context.
         */
        virtual void activate() = 0;

        /**
         * Called every time rendering of the next frame starts.
         */
        virtual void started() = 0;

        /**
         * Called every time frame rendering is complete.
         * @param frame_number the frame number that was rendered. If there is nothing to draw, the parameter will be equal to -1.
         */
        virtual void finished(int64_t frame_number) = 0;
    }; // class render_delegate

} // namespace bnb::player_api::interfaces
