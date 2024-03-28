#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/utils/defs.hpp>

#include <memory>

namespace bnb::player_api::interfaces
{
    class output;
} // namespace bnb::player_api::interfaces

namespace bnb::player_api
{
    using output_sptr = std::shared_ptr<bnb::player_api::interfaces::output>;
} // namespace bnb::player_api

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
         * Activate input.
         */
        virtual void active() = 0;

        /**
         * Deactivate input.
         */
        virtual void deactive() = 0;

        /**
         * Is active.
         * @return true if input is active, otherwise false.
         */
        [[nodiscard]] virtual bool is_active() = 0;

        /**
         * Attach output to the player. Called by the player on the render thread.
         */
        virtual void attach() = 0;

        /**
         * Detach output from the player. Called by the player on the render thread.
         */
        virtual void detach() = 0;

        /**
         * Present the drawn frame by the player to the output
         */
        virtual void present(const output_sptr& self, const render_target_sptr& render_target) = 0;
    }; // class output

} // namespace bnb::player_api::interfaces
