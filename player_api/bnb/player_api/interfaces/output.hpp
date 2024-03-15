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

        void active()
        {
            m_output_is_active = true;
        }

        void deactive()
        {
            m_output_is_active = false;
        }

        [[nodiscard]] bool is_active()
        {
            return m_output_is_active;
        }

        /**
         * Attach output to the player. Called by the player on the render thread.
         */
        virtual void attach() {}

        /**
         * Detach output from the player. Called by the player on the render thread.
         */
        virtual void detach() {}

        /**
         * Present the drawn frame by the player to the output
         */
        virtual void present(const output_sptr& self, const render_target_sptr& render_target) = 0;
    protected:
        bool m_output_is_active {true};
    };

} /* namespace bnb::player_api::interfaces */
