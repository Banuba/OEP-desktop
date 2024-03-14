#include <bnb/player_api/output/opengl_frame_output.hpp>

#include <bnb/player_api/types/yuv_conversion_matrices.hpp>
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

    bnb::player_api::pixel_buffer_sptr allocate_pixel_buffer(bnb::player_api::pixel_buffer_format format, int32_t width, int32_t height)
    {
        using pb_t = bnb::player_api::pixel_buffer;
        uint8_t* data_raw_ptr;

        using t = bnb::player_api::pixel_buffer_format;
        switch (format) {
            case t::bpc8_rgb:
            case t::bpc8_bgr:
                {
                    int32_t stride = (width * 3 + 7) &~ 7;
                    uint8_t* data = new uint8_t[stride * height];
                    return std::make_shared<bnb::player_api::pixel_buffer>(data, stride, format, width, height,
                        [](uint8_t* p) {delete [] p;});
                }
                break;
            case t::bpc8_rgba:
            case t::bpc8_bgra:
            case t::bpc8_argb:
                {
                    int32_t stride = (width * 4 + 7) &~ 7;
                    uint8_t* data = new uint8_t[stride * height];
                    return std::make_shared<bnb::player_api::pixel_buffer>(data, stride, format, width, height,
                        [](uint8_t* p) {delete [] p;});
                }
                break;
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
                {
                    int32_t stride = (width + 15) &~ 15;
                    uint8_t* data = new uint8_t[stride * height * 2];

                    auto* y_data = data;
                    auto* uv_data = data + stride * height;
                    return std::make_shared<bnb::player_api::pixel_buffer>(y_data, stride, uv_data, stride, format, width, height,
                        [](uint8_t* p) {delete [] p;}, [](uint8_t*) {});
                }
                break;
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
                {
                    int32_t stride = (width + 15) &~ 15;
                    uint8_t* data = new uint8_t[stride * height * 2];

                    auto* y_data = data;
                    auto* u_data = data + stride * height;
                    auto* v_data = data + stride * height + ((width / 2 + 7) &~ 7);
                    return std::make_shared<bnb::player_api::pixel_buffer>(y_data, stride, u_data, stride, v_data, stride, format, width, height,
                        [](uint8_t* p) {delete [] p;}, [](uint8_t*) {}, [](uint8_t*) {});
                }
                break;
        }
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
    void opengl_frame_output::present(const render_target_sptr& render_target)
    {
        GL_CALL(glDisable(GL_BLEND));
        GL_CALL(glDisable(GL_CULL_FACE));
        GL_CALL(glDisable(GL_DEPTH_TEST));

        m_shader->use();
        m_shader->set_uniform_texture(m_uniform_texture, static_cast<uint32_t>(render_target->get_output_texture()));
        m_shader->set_uniform_mat4(m_uniform_matrix, get_orientation_matrix());

        auto width = render_target->get_render_width();
        auto height = render_target->get_render_height();
        auto read_width = width;
        auto read_height = height;
        auto pb = allocate_pixel_buffer(m_format, width, height);

        if (m_format_is_bpc8) {
            m_renderbuffer->prepare(width, height);

            GL_CALL(glViewport(0, 0, width, height));
            m_frame_handler->draw_surface();
        } else if (m_format_is_nv12 || m_format_is_i420) {
            auto half_width = width / 2;
            auto half_height = height / 2;
            auto aligned_width = (width + 15) &~ 15;
            auto aligned_half_width = (half_width + 7) &~ 7;
            auto full_height = m_format_is_nv12 ? height * 2 : height + half_height;
            read_width = aligned_width;
            read_height = full_height;
            auto height_indent_for_nv12 = m_format_is_nv12 ? half_height : 0;

            m_renderbuffer->prepare(read_width, read_height);

            GL_CALL(glViewport(0, 0, width, height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_y_plane_convert_coefs);
            m_frame_handler->draw_surface();

            GL_CALL(glViewport(0, height + height_indent_for_nv12, half_width, half_height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_v_plane_convert_coefs);
            m_frame_handler->draw_surface();

            GL_CALL(glViewport(aligned_half_width, height + height_indent_for_nv12, half_width, half_height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_u_plane_convert_coefs);
            m_frame_handler->draw_surface();
        }

        GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 8));
        GL_CALL(glReadPixels(0, 0, read_width, read_height, m_gl_read_pixels_format, GL_UNSIGNED_BYTE, pb->get_base_ptr()));

        if (m_format_is_nv12) {
            libyuv::MergeUVPlane(
                pb->get_base_ptr_of_plane(1) + height / 2 * pb->get_bytes_per_row_of_plane(1),
                pb->get_bytes_per_row_of_plane(1),
                pb->get_base_ptr_of_plane(1) + height / 2 * pb->get_bytes_per_row_of_plane(1) + ((width / 2 + 7) &~ 7),
                pb->get_bytes_per_row_of_plane(1),
                pb->get_base_ptr_of_plane(1),
                pb->get_bytes_per_row_of_plane(1),
                pb->get_width(),
                pb->get_height()
            );
        }

        opengl_renderbuffer::unbind();
        opengl_shader_program::unuse();

        m_pixel_buffer_callback(pb);
    }

} /* namespace bnb::player_api */
