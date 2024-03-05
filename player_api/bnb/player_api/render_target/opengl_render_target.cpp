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
        
    void create_texture(uint32_t& texture, int32_t width, int32_t height)
    {
        GL_CALL(glGenTextures(1, &texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

        GL_CALL(glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_LINEAR));
        GL_CALL(glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MAG_FILTER), GL_LINEAR));
        GL_CALL(glTexParameterf(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_S), GLfloat(GL_CLAMP_TO_EDGE)));
        GL_CALL(glTexParameterf(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_T), GLfloat(GL_CLAMP_TO_EDGE)));
    }
    
    void delete_texture(uint32_t& texture)
    {
        if (texture != 0) {
            GL_CALL(glDeleteTextures(1, &texture));
            texture = 0;
        }
    }

}

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

        GL_CALL(glGenFramebuffers(1, &m_framebuffer));
        
        m_context->deactivate();
    }

    /* opengl_render_target::~opengl_render_target */
    opengl_render_target::~opengl_render_target()
    {
        m_context->activate();
        if (glIsFramebuffer(m_framebuffer)) {
            GL_CALL(glDeleteFramebuffers(1, &m_framebuffer));
            m_framebuffer = 0;
        }
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

        if (m_render_width != width || m_render_height != height) {
            if (m_framebuffer_texture != 0) {
                delete_texture(m_framebuffer_texture);
            }
            create_texture(m_framebuffer_texture, width, height);
            m_render_width = width;
            m_render_height = height;
        }

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebuffer_texture, 0));

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            // TODO: message with status
            std::runtime_error("Failed to make complete framebuffer object: " );
        }

        GL_CALL(glViewport(0, 0, m_render_width, m_render_height));
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
        return m_render_width;
    }

    /* opengl_render_target::get_rendering_height */
    int32_t opengl_render_target::get_render_height() const noexcept
    {
        return m_render_height;
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
        GL_CALL(glDisable(GL_PROGRAM_POINT_SIZE));
        GL_CALL(glViewport(left, top, width, height));
        GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        m_shader->use();

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_framebuffer_texture));
        m_shader->set_uniform_texture(m_uniform_texture, 0);
        m_shader->set_uniform_mat4(m_uniform_matrix, mat4);

        m_frame_handler->draw_surface();

        opengl_shader_program::unuse();
    }
    
    /* opengl_render_target::swap_buffers */
    void opengl_render_target::swap_buffers()
    {
        m_context->swap_buffers();
    }

} /* namespace bnb::player_api */
