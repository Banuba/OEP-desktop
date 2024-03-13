#include <bnb/player_api/render_target/opengl_render_target.hpp>

#include <bnb/effect_player/interfaces/effect_player.hpp>
#include <bnb/effect_player/utility.hpp>

namespace
{

    constexpr std::string_view vertex_shader_source =
        "precision highp float;\n "
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 vTexCoord;\n"
        "uniform mat4 uTextureMatrix;\n"
        "void main() {\n"
        "  gl_Position = uTextureMatrix * vec4(aPos, 1.0);\n"
        "  vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
        "}\n";

    constexpr std::string_view fragment_shader_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "  FragColor = texture(uTexture, vTexCoord);\n"
        "}\n";

} /* namespace */

namespace bnb::player_api
{

    /* opengl_render_target::opengl_render_target */
    opengl_render_target::opengl_render_target(const render_context_sptr& context)
        : m_context(context)
    {
        // This particular example relies on OpenGL, so it should be explicitly requested
        bnb::interfaces::effect_player::set_render_backend(::bnb::interfaces::render_backend_type::opengl);

        bnb::utility::load_gl_functions();

        m_context->activate();

        m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_source);
        m_frame_handler = std::make_unique<opengl_frame_surface_handler>();

        m_shader->use();
        m_uniform_texture = m_shader->get_uniform_location("uTexture");
        m_uniform_matrix = m_shader->get_uniform_location("uTextureMatrix");
        opengl_shader_program::unuse();

        m_renderbuffer = std::make_unique<opengl_renderbuffer>();

        m_context->deactivate();
    }

    /* opengl_render_target::~opengl_render_target */
    opengl_render_target::~opengl_render_target()
    {
        m_context->activate();
        m_renderbuffer = nullptr;
        m_output_renderbuffer = nullptr;
        m_context->deactivate();
    }

    /* opengl_render_target::prepare_to_render */
    void opengl_render_target::prepare_to_render(int32_t width, int32_t height)
    {
        m_context->activate();

        if (width == 0 || height == 0) {
            // just activate context and return
            return;
        }

        m_renderbuffer->prepare(width, height);

        GL_CALL(glViewport(0, 0, width, height));
        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
        GL_CALL(glDisable(GL_CULL_FACE));
        GL_CALL(glDisable(GL_DEPTH_TEST));
        GL_CALL(glEnable(GL_PROGRAM_POINT_SIZE));
    }

    /* opengl_render_target::set_frame_time_us */
    void opengl_render_target::set_frame_time_us(uint64_t time_us) noexcept
    {
        m_frame_time_us = time_us;
    }

    /* opengl_render_target::get_frame_time_us */
    uint64_t opengl_render_target::get_frame_time_us() const noexcept
    {
        return m_frame_time_us;
    }

    /* opengl_render_target::get_rendering_width */
    int32_t opengl_render_target::get_render_width() const noexcept
    {
        return m_renderbuffer->get_width();
    }

    /* opengl_render_target::get_rendering_height */
    int32_t opengl_render_target::get_render_height() const noexcept
    {
        return m_renderbuffer->get_height();
    }

    /* opengl_render_target::get_output_texture */
    texture_t opengl_render_target::get_output_texture() const noexcept
    {
        return reinterpret_cast<texture_t>(m_renderbuffer->get_texture());
    }

    /* opengl_render_target::present */
    void opengl_render_target::present(int32_t left, int32_t top, int32_t width, int32_t height, const float* const mat4)
    {
        m_context->activate();

        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendEquation(GL_FUNC_ADD));
        GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GL_CALL(glDisable(GL_CULL_FACE));
        GL_CALL(glDisable(GL_DEPTH_TEST));
        GL_CALL(glViewport(left, top, width, height));
        GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        m_shader->use();

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_renderbuffer->get_texture()));
        m_shader->set_uniform_texture(m_uniform_texture, 0);
        m_shader->set_uniform_mat4(m_uniform_matrix, mat4);

        m_frame_handler->draw_surface();

        opengl_shader_program::unuse();
    }

    /* opengl_render_target::present_to_screen */
    void opengl_render_target::present_to_screen(int32_t left, int32_t top, int32_t width, int32_t height, const float* const mat4)
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        present(left, top, width, height, mat4);
    }

} /* namespace bnb::player_api */
