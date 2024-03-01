#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/player_api/interfaces/orientation.hpp>

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
        enum class content_mode : int32_t
        {
            /**
             * Positions the drawn frame in the center, while maintaining the aspect ratio.
             * With this layout, the entire frame fits into the drawing area.
             * */
            aspect_fit,
            
            /**
             * Positions the drawn frame in the center, while maintaining the aspect ratio.
             * With this layout, the frame is stretched along the largest of the sides, fills the
             * entire drawing area, while the borders of the drawn frame will be cut off.
             * */
            aspect_fill
        }; /* enum class content_mode */

    public:
        virtual ~output() = default;

        /**
         * Set new content mode. By default the content mode is set to aspect_fill
         * @param new_content_mode rendering logic when the size of the surface and size of the
         * frame are different
         */
        virtual void set_content_mode(content_mode new_content_mode) = 0;

        /**
         * Apply output frame orientation. The nearest output frame will have this orientation.
         * @param orientation rotation of output frame
         * @param mirroring require mirroring
         */
        virtual void set_orientation(orientation orient, bool mirroring) = 0;

        /**
         * Present the drawn frame by the player to the output
         */
        virtual void present(const render_target_sptr render_target) = 0;
    };

} /* namespace bnb::player_api::interfaces */
