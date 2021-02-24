#include "renderer.hpp"
#include "opengl.hpp"

//NV12
namespace
{
    const char* vs = \
        " precision highp float; \n "
        " layout (location = 0) in vec3 aPos; \n"
        " layout (location = 1) in vec2 aTexCoord; \n"
        " out vec2 TexCoord; \n"
        " void main() \n"
        " { \n"
            " gl_Position = vec4(aPos, 1.0); \n"
            " TexCoord = aTexCoord; \n"
        " } \n";

    const char* fs = \
        "precision highp float; \n "
        "out vec4 FragColor; \n"
        "in vec2 TexCoord; \n"
        "uniform sampler2D yTexture; \n"
        "uniform sampler2D uvTexture; \n"

        "void main() \n"
        "{ \n"
            "float r, g, b, y, u, v; \n"
            "float Umax = 0.436; \n"
            "float Vmax = 0.615; \n"
            "float Wr = 0.299; \n"
            "float Wb = 0.114; \n"
            "float Wg = 1. - Wr - Wb; \n"
            "y = texture(yTexture, TexCoord).x; \n"
            "u = texture(uvTexture, TexCoord).x - 0.5; \n"
            "v = texture(uvTexture, TexCoord).y - 0.5; \n"

            "r = y + v * ((1. - Wr) / Vmax); \n"
            "g = y - u * ((Wb * (1. - Wb)) / (Umax * Wg)) - v * ((Wr * (1. - Wr)) / (Vmax * Wg)); \n"
            "b = y + u * ((1. - Wb)/Umax); \n"
            "FragColor = vec4(r, g, b, 1.0); \n"
        "} \n";
}

namespace bnb::render
{
    using gl_context = renderer_gl_context;

    renderer::renderer(int width, int height) :
        m_program("RendererCamera", vs, fs)
    {
        GL_CALL(glGenTextures(gl_context::textures_amount, m_gl_context.textures));
        surface_change(width, height);
    }

    void renderer::surface_change(int32_t width, int32_t height)
    {
        m_width = width;
        m_height = height;
        m_surface_changed = true;
    }

    void renderer::update_data(full_image_t image)
    {
        if (m_texture_updated && m_rendering) {
            return;
        }

        m_texture_updated = false;
        const auto& yuv = image.get_data<yuv_image_t>();
        m_update_buffer.y_plane = yuv.y_plane;
        m_update_buffer.uv_plane = yuv.uv_plane;
        m_texture_updated = true;
    }

    bool renderer::draw()
    {
        if (!m_texture_updated) {
            return false;
        }

        if (m_surface_changed) {
            GL_CALL(glViewport(0, 0, m_width, m_height));
            m_surface_changed = false;
        }

        m_rendering = true;

        std::swap(m_update_buffer, m_show_buffer);
        m_texture_updated = false;

        update_camera_texture();

        m_program.use();

        m_gl_context.texture_uniform_location[SamplerIndex::Y] = glGetUniformLocation(m_program.handle(), SamplerName::Y);
        m_gl_context.texture_uniform_location[SamplerIndex::UV] = glGetUniformLocation(m_program.handle(), SamplerName::UV);

        for (auto i = 0u; i < gl_context::textures_amount; i++) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + i));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_gl_context.textures[i]));
            GL_CALL(glUniform1i(m_gl_context.texture_uniform_location[i], i));
        }

        m_gl_context.m_frame_surface.draw();

        m_program.unuse();
        m_rendering = false;

        return true;
    }

    void renderer::update_camera_texture()
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_gl_context.textures[SamplerIndex::Y]));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG
                    , m_width, m_height, 0,
                    GL_RED, GL_UNSIGNED_BYTE, m_show_buffer.y_plane.get()));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_gl_context.textures[SamplerIndex::UV]));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG
                        , m_width / 2, m_height / 2, 0,
                    GL_RG, GL_UNSIGNED_BYTE, m_show_buffer.uv_plane.get()));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
} // bnb::render