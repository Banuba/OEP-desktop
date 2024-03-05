#include <bnb/player_api/render_target/opengl_render_target.hpp>

#include <bnb/effect_player/interfaces/all.hpp>

namespace
{
    constexpr std::string_view vertex_shader_source =
        "precision highp float;\n "
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 vTexCoord;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos, 1.0);\n"
        "  vTexCoord = aTexCoord;\n"
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
    opengl_render_target::opengl_render_target(effect_player_sptr effect_player, render_context_sptr context)
        : m_effect_player(effect_player)
        , m_context(context)
        , m_shader(std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_source))
        , m_frame_handler(std::make_unique<opengl_frame_surface_handler>())
    {
        m_context->activate();
        GL_CALL(glGenFramebuffers(1, &m_framebuffer));
    }

    /* opengl_render_target::~opengl_render_target */
    opengl_render_target::~opengl_render_target()
    {
        m_context->activate();
        m_shader = nullptr;
        m_frame_handler = nullptr;
        if (glIsFramebuffer(m_framebuffer)) {
            GL_CALL(glDeleteFramebuffers(1, &m_framebuffer));
            m_framebuffer = 0;
        }
        m_context->deactivate();
        m_context = nullptr;
    }

    /* opengl_render_target::prepare_to_render */
    void opengl_render_target::prepare_to_render()
    {
        m_context->activate();

        auto effect_manager = m_effect_player->effect_manager();
        auto w = effect_manager->surface_size().width;
        auto h = effect_manager->surface_size().height;

        if (m_render_width != w || m_render_height != h) {
            if (m_framebuffer_texture != 0) {
                delete_texture(m_framebuffer_texture);
            }
            create_texture(m_framebuffer_texture, w, h);
            m_render_width = w;
            m_render_height = h;
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
    void opengl_render_target::present(int32_t left, int32_t top, int32_t width, int32_t height)
    {
        m_context->activate();
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendEquation(GL_FUNC_ADD));
        GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GL_CALL(glViewport(left, top, width, height));
        GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        m_shader->use();

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_framebuffer_texture));
        m_shader->set_uniform_texture(m_shader->get_uniform_location("uTexture"), 0);

        m_frame_handler->draw_surface();

        opengl_shader_program::unuse();

        m_context->swap_buffers();
    }
    
} /* namespace bnb::player_api */
