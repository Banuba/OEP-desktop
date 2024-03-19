#pragma once

#include <bnb/player_api/opengl/opengl_renderbuffer.hpp>
#include <bnb/player_api/opengl/opengl_shader_program.hpp>
#include <bnb/player_api/opengl/opengl_frame_surface_handler.hpp>
#include <bnb/player_api/output/output_base.hpp>
#include <bnb/player_api/pixel_buffer/pixel_buffer.hpp>

#include <functional>

namespace bnb::player_api
{

    class opengl_frame_output : public output_base
    {
    public:
        using pixel_buffer_callback = std::function<void(const output_sptr& self, const pixel_buffer_sptr& pb)>;

    public:
        opengl_frame_output(const pixel_buffer_callback& callback, pixel_buffer_format format);

        ~opengl_frame_output() = default;

        void attach() override;

        void detach() override;

        void present(const output_sptr& self, const render_target_sptr& render_target) override;

    private:
        void prepare_to_render(int32_t framebuffer_width, int32_t framebuffer_height);

        void render(int32_t left, int32_t top, int32_t width, int32_t height);

    private:
        pixel_buffer_callback m_pixel_buffer_callback;
        pixel_buffer_format m_format;
        std::unique_ptr<opengl_renderbuffer> m_renderbuffer;
        std::unique_ptr<opengl_shader_program> m_shader;
        std::unique_ptr<opengl_frame_surface_handler> m_frame_handler;

        const float* m_y_plane_convert_coefs {nullptr};
        const float* m_u_plane_convert_coefs {nullptr};
        const float* m_v_plane_convert_coefs {nullptr};

        bool m_format_is_bpc8 {false};
        bool m_format_is_nv12 {false};
        bool m_format_is_i420 {false};

        uint32_t m_gl_read_pixels_format {0};

        int32_t m_uniform_texture {0};
        int32_t m_uniform_matrix {0};
        int32_t m_uniform_yuv_plane_convert_coefs {0};
    }; /* class opengl_frame_output */

} /* namespace bnb::player_api */
