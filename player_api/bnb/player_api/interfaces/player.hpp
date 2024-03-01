#pragma once

#include <bnb/player_api/interfaces/input.hpp>
#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/effect_player/interfaces/effect_player.hpp>
#include <bnb/effect_player/interfaces/effect.hpp>

#include <memory>

namespace bnb::player_api::interfaces
{
    class player;
} /* namespace bnb::player_api::interfaces */

namespace bnb::player_api
{
    using player_sptr = std::shared_ptr<bnb::player_api::interfaces::player>;
    using effect_sptr = std::shared_ptr<bnb::interfaces::effect>;
    using js_callback_sptr = std::shared_ptr<bnb::interfaces::js_callback>;
    using effect_player_sptr = std::shared_ptr<bnb::interfaces::effect_player>;
} /* namespace bnb::player_api */

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

    public:
        virtual ~player() = default;

        /**
         * Set rendering mode to another. By default, the render mode is set to LOOP.
         * @param new_render_mode new rendering mode
         */
        virtual void set_render_mode(render_mode new_render_mode) = 0;

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
         * Use new input
         * @param input frames will be receive from it
         */
        virtual void use(const input_sptr input) = 0;

        /**
         * Use single new output and remove old
         * @param output processed frames will be push to it
         */
        virtual void use(const output_sptr output) = 0;

        /**
         * Use single new Output and remove old
         * @param output processed frames will be push to it
         */
        virtual void use(const std::vector<output_sptr> outputs) = 0;

        /**
         * Use single input and single output.
         * @param input frames will be receive from it
         * @param output processed frames will be push to it
         */
        virtual void use(const input_sptr input, const output_sptr output) = 0;

        /**
         * Use single Input and multiple Outputs.
         * @param input frames will be receive from it
         * @param outputs processed frames will be push to these outputs
         */
        virtual void use(const input_sptr input, const std::vector<output_sptr> outputs) = 0;

        /**
         * Add a new one output to output list.
         * @param output processed frames will be push to it
         */
        virtual void add_output(const output_sptr output) = 0;

        /**
         * Remove one output from output list.
         * @param output delete this output from outputs
         */
        virtual void remove_output(const output_sptr output) = 0;

        /**
         * Synchronous loading of an effect by name
         * @param url path to the effect or effect name
         */
        virtual effect_sptr load(const std::string & url) = 0;

        /**
         * Load effect asynchronously by name
         * @param url path to the effect or effect name
         */
        virtual effect_sptr load_async(const std::string & url) = 0;

        /**
         * Evaluate the `script` in effect
         * @param script JS string to execute
         * @param resultCallback Callback for result, will be called in render thread.
         */
        virtual void eval_js(const std::string& script, js_callback_sptr callback) = 0;

        /**
         * Draw and present rendered result synchronously, can be used only in `manual` rendering mode.
         * @return `false` when no new data in the `input`, `input` or `outputs` is null
         */
        virtual bool render() = 0;
    };

} /* bnb::player_api::interfaces */
