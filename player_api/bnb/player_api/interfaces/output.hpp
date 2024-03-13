#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>

#include <memory>

namespace bnb::player_api::interfaces
{
    class output;
} /* namespace bnb::player_api::interfaces */

namespace bnb::player_api
{
    using output_sptr = std::shared_ptr<bnb::player_api::interfaces::output>;
} /* namespace bnb::player_api */

namespace bnb::player_api::interfaces
{

    /**
     * The interface is inherited by all classes that must work with player,
     * and responsible for passing the rendered frame to the output view
     */
    class output
    {
    public:
        virtual ~output() = default;

        /**
         * Present the drawn frame by the player to the output
         */
        virtual void present(const render_target_sptr& render_target) = 0;
    };

} /* namespace bnb::player_api::interfaces */
