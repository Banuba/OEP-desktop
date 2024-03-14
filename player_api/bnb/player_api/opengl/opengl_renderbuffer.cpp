#include <bnb/player_api/opengl/opengl_renderbuffer.hpp>

#include <bnb/player_api/opengl/opengl.hpp>

#include <stdexcept>

namespace bnb::player_api
{

    /* opengl_renderbuffer::opengl_renderbuffer */
    opengl_renderbuffer::opengl_renderbuffer()
    {
         GL_CALL(glGenFramebuffers(1, &m_framebuffer));
    }

    /* opengl_renderbuffer::~opengl_renderbuffer */
    opengl_renderbuffer::~opengl_renderbuffer()
    {
        if (m_framebuffer != 0) {
            GL_CALL(glDeleteFramebuffers(1, &m_framebuffer));
            m_framebuffer = 0;
        }
        if (m_texture != 0) {
            GL_CALL(glDeleteTextures(1, &m_texture));
            m_texture = 0;
        }
    }

    /* opengl_renderbuffer::prepare */
    void opengl_renderbuffer::prepare(int32_t width, int32_t height)
    {
        if (m_width != width || m_height != height) {
            if (m_texture != 0) {
                GL_CALL(glDeleteTextures(1, &m_texture));
                m_texture = 0;
            }

            GL_CALL(glGenTextures(1, &m_texture));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_texture));
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

            GL_CALL(glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_LINEAR));
            GL_CALL(glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MAG_FILTER), GL_LINEAR));
            GL_CALL(glTexParameterf(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_S), GLfloat(GL_CLAMP_TO_EDGE)));
            GL_CALL(glTexParameterf(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_T), GLfloat(GL_CLAMP_TO_EDGE)));

            m_width = width;
            m_height = height;
        }

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0));

        GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        if (GLenum status; (status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
            std::runtime_error("Failed to make complete framebuffer object.");
        }
    }

    /* opengl_renderbuffer::unbind */
    void opengl_renderbuffer::unbind()
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    /* opengl_renderbuffer::get_width */
    int32_t opengl_renderbuffer::get_width() const noexcept
    {
        return m_width;
    }

    /* opengl_renderbuffer::get_height */
    int32_t opengl_renderbuffer::get_height() const noexcept
    {
        return m_height;
    }

    /* opengl_renderbuffer::get_texture */
    uint32_t opengl_renderbuffer::get_texture() const noexcept
    {
        return m_texture;
    }

} /* namespace bnb::player_api */
