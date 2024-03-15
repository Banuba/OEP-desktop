#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/player_api/interfaces/render_context.hpp>
#include <bnb/player_api/opengl/opengl_shader_program.hpp>
#include <bnb/player_api/opengl/opengl_frame_surface_handler.hpp>
#include <bnb/player_api/opengl/opengl_renderbuffer.hpp>

namespace bnb::player_api
{

    class opengl_render_target : public bnb::player_api::interfaces::render_target
    {
    public:
        opengl_render_target(const render_context_sptr& context);

        ~opengl_render_target() override;

        void prepare_to_offscreen_render(int32_t width, int32_t height) override;

        void prepare_to_screen_render() override;

        void set_frame_time_us(uint64_t time_us) noexcept override;

        uint64_t get_frame_time_us() const noexcept override;

        int32_t get_render_width() const noexcept override;

        int32_t get_render_height() const noexcept override;

        texture_t get_output_texture() const noexcept override;

        void present(int32_t left, int32_t top, int32_t width, int32_t height, const float* const mat4) override;

    private:
        render_context_sptr m_context;
        std::unique_ptr<opengl_shader_program> m_shader;
        std::unique_ptr<opengl_shader_program> m_shader_y_flip;
        std::unique_ptr<opengl_frame_surface_handler> m_frame_handler;
        std::unique_ptr<opengl_renderbuffer> m_renderbuffer;

        uint64_t m_frame_time_us {0};

        int32_t m_uniform_texture {0};
        int32_t m_uniform_matrix {0};
        int32_t m_uniform_invert_y {0};
    }; /* class opengl_render_target */

} /* namespace bnb::player_api */
