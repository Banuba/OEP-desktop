#pragma once

#include <bnb/player_api/interfaces/input.hpp>
#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/effect_player/interfaces/effect_player.hpp>
#include <bnb/effect_player/interfaces/effect.hpp>
#include <bnb/utils/defs.hpp>

#include <memory>

namespace bnb::player_api::interfaces
{
    class player;
} // namespace bnb::player_api::interfaces

namespace bnb::player_api
{
    using player_sptr = std::shared_ptr<bnb::player_api::interfaces::player>;
    using effect_sptr = std::shared_ptr<bnb::interfaces::effect>;
    using js_callback_sptr = std::shared_ptr<bnb::interfaces::js_callback>;
    using effect_player_sptr = std::shared_ptr<bnb::interfaces::effect_player>;
} // namespace bnb::player_api

namespace bnb::player_api::interfaces
{

    /**
     * Class manages the lifecycle of the EffectPlayer and is responsible for drawing FrameData
     * that comes from Input, then processing frame and sending it to Output (or Outputs).
     * Owns and manages the render thread.
     */
    class player
    {
    public:
        enum class render_mode : int32_t
        {
            /**
             *Render in display linked loop with defined `fps`
             */
            loop,
            /**
             * Render manually by `render` call
             */
            manual
        };

        class rendering_process
        {
        public:
            virtual ~rendering_process() = default;

            virtual void started() = 0;

            virtual void frame_rendered(int64_t frame_number) = 0;

            virtual void finished() = 0;
        }; // class rendering_pipeline

    public:
        virtual ~player() = default;

        /**
         * Set rendering mode to another. By default, the render mode is set to LOOP.
         * @param new_render_mode new rendering mode
         */
        virtual void set_render_mode(render_mode new_render_mode) = 0;

        /**
         * Set rendering callback status
         * @param callback render callback
         */
        virtual void set_rendering_process_callback(const std::shared_ptr<rendering_process>& callback) = 0;

        /**
         * Resume the playback of the effect.
         */
        virtual void play() = 0;

        /**
         * Pause the playback of the effect.
         */
        virtual void pause() = 0;

        /**
         * Get an instance of the EffectPlayer
         */
        virtual effect_player_sptr get_effect_player() = 0;

        /**
         * Use the new input to replace the old one
         * @param input frames will be receive from it
         */
        virtual player& in(const input_sptr& input) = 0;

        /**
         * Add a new one output to output list
         * @param output processed frames will be push to it
         */
        virtual player& out(const output_sptr& output) = 0;

        /**
         * Use output for one frame and remove it
         * @param outputs single processed frames will be push to these outputs
         */
        virtual player& out_once(const output_sptr& output) = 0;

        /**
         * Remove one output. If the output `nullptr` is passed as an output, then all outputs are removed.
         * @param output delete this output from outputs
         */
        virtual player& remove_out(const output_sptr& output = nullptr) = 0;

        /**
         * Synchronous loading of an effect by name
         * @param url path to the effect or effect name
         */
        virtual effect_sptr load(const std::string& url) = 0;

        /**
         * Load effect asynchronously by name
         * @param url path to the effect or effect name
         */
        virtual effect_sptr load_async(const std::string& url) = 0;

        /**
         * Evaluate the `script` in effect
         * @param script JS string to execute
         * @param resultCallback Callback for result, will be called in render thread.
         */
        virtual void eval_js(const std::string& script, js_callback_sptr callback) = 0;

        /**
         * Draw and present rendered result synchronously, can be used only in `manual` rendering mode.
         * @return returns the number of the drawn frame, or -1 if nothing could be drawn.
         */
        virtual int64_t render() = 0;
    }; // class player

} // namespace bnb::player_api::interfaces
