#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/player_api/interfaces/player.hpp>
#include <bnb/player_api/opengl/opengl.hpp>
#include <bnb/player_api/opengl/opengl_context.hpp>
#include <bnb/player_api/opengl/opengl_shader_program.hpp>
#include <bnb/player_api/opengl/opengl_frame_surface_handler.hpp>


namespace bnb::player_api
{

    class opengl_render_target : public bnb::player_api::interfaces::render_target
    {
    public:
        opengl_render_target(effect_player_sptr effect_player, std::shared_ptr<opengl_context> context);

        ~opengl_render_target() override;

        void prepare_to_render() override;

        void set_frame_time_us(uint64_t time_us) noexcept override;

        uint64_t get_frame_time_us() const noexcept override;

        int32_t get_render_width() const noexcept override;

        int32_t get_render_height() const noexcept override;

        void present(int32_t left, int32_t top, int32_t width, int32_t height) override;

    private:
        effect_player_sptr m_effect_player;
        std::shared_ptr<opengl_context> m_context;
        std::unique_ptr<opengl_shader_program> m_shader;
        std::unique_ptr<opengl_frame_surface_handler> m_frame_handler;
        
        uint64_t m_frame_time_us {0};
        int32_t m_render_width {0};
        int32_t m_render_height {0};
        
        uint32_t m_framebuffer {0};
        uint32_t m_framebuffer_texture {0};
    }; /* class opengl_render_target */

} /* namespace bnb::player_api */
