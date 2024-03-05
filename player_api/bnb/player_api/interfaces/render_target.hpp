#pragma once

#include <memory>

namespace bnb::player_api::interfaces
{
    class render_target;
} /* namespace bnb::player_api::interfaces */

namespace bnb::player_api
{
    using render_target_sptr = std::shared_ptr<bnb::player_api::interfaces::render_target>;
} /* namespace bnb::player_api */

namespace bnb::player_api::interfaces
{

    /**
     * Responsible for preparation for drawing frames to 'outputs'.
     * This part of the RenderTarget is owned and controlled only by the player
     */
    class render_target
    {
    public:
        /**
         * Destroy render_target and release resources
         */
        virtual ~render_target() = default;

        /**
         * Prepare render target to render
         *  @param width rendering surface width
         *  @param height rendering surface height
         */
        virtual void prepare_to_render(int32_t width, int32_t height) = 0;

        /**
         * Set presentation frame time
         * @param time_us time in microseconds
         */
        virtual void set_frame_time_us(uint64_t time_us) noexcept = 0;

        /**
         * Get frame time microseconds
         */
        virtual uint64_t get_frame_time_us() const noexcept = 0;

        /**
         * Get render surface size width
         */
        virtual int32_t get_render_width() const noexcept = 0;

        /**
         * Get render surface size height
         */
        virtual int32_t get_render_height() const noexcept = 0;

        /**
         * Draw frame on surface
         * @param left 
         * @param top
         * @param width
         * @param height
         */
        virtual void present(int32_t left, int32_t top, int32_t width, int32_t height, const float* const mat4) = 0;

        /**
         * Swap buffers
         */
        virtual void swap_buffers() = 0;
    };

} /* namespace bnb::player_api::interfaces */