#include <bnb/player_api/output/opengl_frame_output.hpp>

#include <bnb/player_api/types/yuv_conversion.hpp>
#include <bnb/player_api/opengl/opengl.hpp>
#include <libyuv.h>

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
        "  vTexCoord = aTexCoord;\n"
        "}\n";

    constexpr std::string_view fragment_shader_rgb_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "  FragColor = texture(uTexture, vTexCoord);\n"
        "}\n";
    
    constexpr std::string_view fragment_shader_bgr_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "  FragColor = texture(uTexture, vTexCoord).bgra;\n"
        "}\n";

    constexpr std::string_view fragment_shader_argb_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "  FragColor = texture(uTexture, vTexCoord).argb;\n"
        "}\n";

    constexpr std::string_view fragment_shader_i420_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform vec4 uPlaneConversionCoefs;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "    float a = uPlaneConversionCoefs.a;\n"
        "    vec3 rgb = uPlaneConversionCoefs.rgb;\n"
        "    FragColor.r = a + dot(rgb, texture(uTexture, vTexCoord).rgb);\n"
        "}\n";

    int32_t align_by_8(int32_t x)
    {
        return (x + 7) &~ 7;
    }

    int32_t align_by_16(int32_t width)
    {
        return (width + 15) &~ 15;
    }

    int32_t align_by_32(int32_t width)
    {
        return (width + 31) &~ 31;
    }

    uint8_t* alloc_pixels(size_t size)
    {
        return reinterpret_cast<uint8_t*>(std::aligned_alloc(32, size));
    }

    void dealloc_pixels(uint8_t* p)
    {
        std::free(p);
    }

    bnb::player_api::pixel_buffer_sptr allocate_pixel_buffer(
        bnb::player_api::pixel_buffer_format format,
        int32_t width,
        int32_t height,
        bnb::player_api::orientation orient,
        bool mirror
    )
    {
        using pb_t = bnb::player_api::pixel_buffer;
        using t = bnb::player_api::pixel_buffer_format;
        switch (format) {
            case t::bpc8_rgb:
            case t::bpc8_bgr:
                {
                    auto stride = align_by_8(width * 3);
                    auto* data = alloc_pixels(stride * height);
                    return std::make_shared<pb_t>(data, stride, width, height, format, orient, mirror, dealloc_pixels);
                }
                break;
            case t::bpc8_rgba:
            case t::bpc8_bgra:
            case t::bpc8_argb:
                {
                    auto stride = align_by_8(width * 4);
                    auto* data = alloc_pixels(stride * height);
                    return std::make_shared<pb_t>(data, stride, width, height, format, orient, mirror, dealloc_pixels);
                }
                break;
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
                {
                    auto stride = align_by_32(width);
                    auto* data = alloc_pixels(stride * height + stride * bnb::player_api::uv_plane_height(height) + stride);
                    auto* uv_data = data + stride * height;
                    return std::make_shared<pb_t>(data, stride, uv_data, stride, width, height, format, orient, mirror, dealloc_pixels, nullptr);
                }
                break;
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
                {
                    auto stride = align_by_32(width);
                    auto* data = alloc_pixels(stride * height + stride * bnb::player_api::uv_plane_height(height));
                    auto* u_data = data + stride * height;
                    auto* v_data = data + stride * height + align_by_16(bnb::player_api::uv_plane_width(width));
                    return std::make_shared<pb_t>(data, stride, u_data, stride, v_data, stride, width, height, format, orient, mirror, dealloc_pixels, nullptr, nullptr);
                }
                break;
        }
        return nullptr;
    }

} /* namespace */

namespace bnb::player_api
{

    /* opengl_frame_output::frame_output */
    opengl_frame_output::opengl_frame_output(const pixel_buffer_callback& callback, pixel_buffer_format format)
        : m_pixel_buffer_callback(callback)
        , m_format(format)
        , m_format_is_bpc8(pixel_buffer_format_is_bpc8(format))
        , m_format_is_nv12(pixel_buffer_format_is_nv12(format))
        , m_format_is_i420(pixel_buffer_format_is_i420(format))
    {
        using t = bnb::player_api::pixel_buffer_format;
        if (m_format_is_bpc8) {
            m_gl_read_pixels_format = format == t::bpc8_rgb || format == t::bpc8_bgr ? GL_RGB : GL_RGBA;
        } else if (m_format_is_nv12 || m_format_is_i420) {
            m_gl_read_pixels_format = GL_RED;
            const auto* const mat = get_conversion_matrix_from_rgb_to_yuv(m_format);
            m_y_plane_convert_coefs = mat + yuv_offset_to_y_coeffs;
            m_u_plane_convert_coefs = mat + yuv_offset_to_u_coeffs;
            m_v_plane_convert_coefs = mat + yuv_offset_to_v_coeffs;
        }
    }

    /* opengl_frame_output::attach */
    void opengl_frame_output::attach()
    {
        m_renderbuffer = std::make_unique<opengl_renderbuffer>();
        m_frame_handler = std::make_unique<opengl_frame_surface_handler>();

        using t = bnb::player_api::pixel_buffer_format;
        switch (m_format) {
            case t::bpc8_rgb:
            case t::bpc8_rgba:
                m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_rgb_source);
                break;
            case t::bpc8_bgr:
            case t::bpc8_bgra:
                m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_bgr_source);
                break;
            case t::bpc8_argb:
                m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_argb_source);
                break;
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
                m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_i420_source);
                m_shader->use();
                m_uniform_yuv_plane_convert_coefs = m_shader->get_uniform_location("uPlaneConversionCoefs");
                break;
        }

        m_shader->use();
        m_uniform_texture = m_shader->get_uniform_location("uTexture");
        m_uniform_matrix = m_shader->get_uniform_location("uTextureMatrix");
        opengl_shader_program::unuse();
    }

    /* opengl_frame_output::detach */
    void opengl_frame_output::detach()
    {
        m_renderbuffer = nullptr;
        m_frame_handler = nullptr;
        m_shader = nullptr;
    }

    /* opengl_frame_output::present */
    void opengl_frame_output::present(const output_sptr& self, const render_target_sptr& render_target)
    {
        m_shader->use();
        m_shader->set_uniform_texture(m_uniform_texture, static_cast<uint32_t>(render_target->get_output_texture()));
        m_shader->set_uniform_mat4(m_uniform_matrix, get_orientation_matrix(true));

        int32_t width, height;
        oriented_frame_size(render_target, width, height);
        auto renderbuffer_width = width;
        auto renderbuffer_height = height;
        auto pb = allocate_pixel_buffer(m_format, width, height, m_orientation, m_mirroring);

        if (m_format_is_bpc8) {
            m_renderbuffer->prepare(renderbuffer_width, renderbuffer_height);
            GL_CALL(glDisable(GL_BLEND));
            GL_CALL(glDisable(GL_CULL_FACE));
            GL_CALL(glDisable(GL_DEPTH_TEST));

            GL_CALL(glViewport(0, 0, width, height));
            m_frame_handler->draw_surface();
        } else if (m_format_is_nv12 || m_format_is_i420) {
            auto uv_width = uv_plane_width(width);
            auto uv_height = uv_plane_height(height);
            renderbuffer_width = align_by_32(width);
            renderbuffer_height = height + uv_height + static_cast<int32_t>(m_format_is_nv12);
            // for optimization so that when converting to nv12 there are no additional memory allocations
            auto height_indent_for_nv12 = static_cast<int32_t>(m_format_is_nv12);

            m_renderbuffer->prepare(renderbuffer_width, renderbuffer_height);
            GL_CALL(glDisable(GL_BLEND));
            GL_CALL(glDisable(GL_CULL_FACE));
            GL_CALL(glDisable(GL_DEPTH_TEST));

            GL_CALL(glViewport(0, 0, width, height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_y_plane_convert_coefs);
            m_frame_handler->draw_surface();

            GL_CALL(glViewport(0, height + height_indent_for_nv12, uv_width, uv_height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_u_plane_convert_coefs);
            m_frame_handler->draw_surface();

            GL_CALL(glViewport(align_by_16(uv_width), height + height_indent_for_nv12, uv_width, uv_height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_v_plane_convert_coefs);
            m_frame_handler->draw_surface();
        }

        GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 8));
        GL_CALL(glReadPixels(0, 0, renderbuffer_width, renderbuffer_height, m_gl_read_pixels_format, GL_UNSIGNED_BYTE, pb->get_base_ptr()));

        if (m_format_is_nv12) {
            libyuv::MergeUVPlane(
                pb->get_base_ptr_of_plane(1) + pb->get_bytes_per_row_of_plane(1),
                pb->get_bytes_per_row_of_plane(1),
                pb->get_base_ptr_of_plane(1) + pb->get_bytes_per_row_of_plane(1) + align_by_16(uv_plane_width(pb->get_width())),
                pb->get_bytes_per_row_of_plane(1),
                pb->get_base_ptr_of_plane(1),
                pb->get_bytes_per_row_of_plane(1),
                pb->get_width_of_plane(1),
                pb->get_height_of_plane(1)
            );
        }

        opengl_renderbuffer::unbind();
        opengl_shader_program::unuse();

        m_pixel_buffer_callback(self, pb);
    }

} /* namespace bnb::player_api */
