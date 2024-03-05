#pragma once

#include <memory>

namespace bnb::player_api::interfaces
{
    class render_context;
} /* namespace bnb::player_api::interfaces */

namespace bnb::player_api
{
    using render_context_sptr = std::shared_ptr<bnb::player_api::interfaces::render_context>;
} /* namespace bnb::player_api */

namespace bnb::player_api::interfaces
{

    class render_context
    {
    public:
        /**
         * Destroy render_context and release resources
         */
        virtual ~render_context() = default;

        virtual void activate() = 0;

        virtual void deactivate() = 0;

        virtual void swap_buffers() = 0;
    };

} /* namespace bnb::player_api::interfaces */
