#include <bnb/player_api/output/opengl_frame_output.hpp>

#include <bnb/player_api/opengl/opengl.hpp>
#include <bnb/types/full_image.hpp>
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

    constexpr std::string_view fragment_shader_yuv_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform vec2 uPixelStep;\n"
        "uniform vec4 uPlaneConversionCoefs;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "    float a = uPlaneConversionCoefs.a;\n"
        "    vec3 rgb = uPlaneConversionCoefs.rgb;\n"
        "    FragColor.r = a + dot(rgb, texture(uTexture, vTexCoord - 1.5 * uPixelStep).rgb);\n"
        "    FragColor.g = a + dot(rgb, texture(uTexture, vTexCoord - 0.5 * uPixelStep).rgb);\n"
        "    FragColor.b = a + dot(rgb, texture(uTexture, vTexCoord + 0.5 * uPixelStep).rgb);\n"
        "    FragColor.a = a + dot(rgb, texture(uTexture, vTexCoord + 1.5 * uPixelStep).rgb);\n"
        "}\n";

    constexpr uint32_t offset_to_y_coeffs = 0;
    constexpr uint32_t offset_to_u_coeffs = 4;
    constexpr uint32_t offset_to_v_coeffs = 8;

    constexpr uint32_t offset_to_r_coeffs = 0;
    constexpr uint32_t offset_to_g_coeffs = 4;
    constexpr uint32_t offset_to_b_coeffs = 8;

    const float* const get_conversion_matrix_from_rgb_to_yuv(bnb::player_api::pixel_buffer_format to_yuv_format)
    {
        // The base values for calculating coefficients were taken from the corresponding standard.
        // See link to WIKI: https://en.wikipedia.org/wiki/YUV
        // clang-format off
        static const float mat_cvt_from_rgb_to_bt601_video_range[] = {
             0.256788f,  0.504129f,  0.0979059f, 0.0627451f, // Y plane coefficients
            -0.148223f, -0.290993f,  0.439216f,  0.501961f,  // U plane coefficients
             0.439216f, -0.367788f, -0.0714274f, 0.501961f   // V plane coefficients
        };
        static const float mat_cvt_from_rgb_to_bt601_full_range[] = {
             0.2990000f,     0.587000f,      0.1140000f,     0.000000f, // Y plane coefficients
            -0.1687358916f, -0.3312641084f,  0.50f,          0.50f,     // U plane coefficients
             0.50f,         -0.4186875892f, -0.08131241084f, 0.50f      // V plane coefficients
        };
        static const float mat_cvt_from_rgb_to_bt709_video_range[] = {
             0.1825858824f,  0.6142305882f,  0.06200705882f, 0.06274509804f, // Y plane coefficients
            -0.1006437324f, -0.3385719539f,  0.4392156863f,  0.5019607843f,  // U plane coefficients
             0.4392156863f, -0.3989421626f, -0.04027352368f, 0.5019607843f   // V plane coefficients
        };
        static const float mat_cvt_from_rgb_to_bt709_full_range[] = {
             0.212600000f,   0.71520000f,    0.07220000f,    0.00000000f,   // Y plane coefficients
            -0.1145721061f, -0.3854278939f,  0.5f,           0.5019607843f, // U plane coefficients
             0.5f,          -0.4541529083f, -0.04584709169f, 0.5019607843f  // V plane coefficients
        };
        // clang-format on

        using t = bnb::player_api::pixel_buffer_format;
        switch (to_yuv_format) {
            case t::nv12_bt601_full:
            case t::i420_bt601_full:
                return mat_cvt_from_rgb_to_bt601_full_range;
            case t::nv12_bt601_video:
            case t::i420_bt601_video:
                return mat_cvt_from_rgb_to_bt601_video_range;
            case t::nv12_bt709_full:
            case t::i420_bt709_full:
                return mat_cvt_from_rgb_to_bt709_full_range;
            case t::nv12_bt709_video:
            case t::i420_bt709_video:
                return mat_cvt_from_rgb_to_bt709_video_range;
            default:
                throw std::logic_error("there is no conversion matrix for this type.");
        }
    }

    const float* const get_conversion_matrix_from_yuv_to_rgb(bnb::player_api::pixel_buffer_format from_yuv_format)
    {
        // The base values for calculating coefficients were taken from the corresponding standard.
        // See link to WIKI: https://en.wikipedia.org/wiki/YUV
        // clang-format off
        static const float mat_cvt_from_bt601_full_range_to_rgb[] = {
            1.0f,  0.0000000000f,  1.4020000000f, -0.7037490196f, /* RED coeffs */
            1.0f, -0.3441362862f, -0.7141362862f,  0.5312113305f, /* GREEN coeffs */
            1.0f,  1.7720000000f,  0.0000000000f, -0.8894745098f  /* BLUE coeffs */
        };
        static const float mat_cvt_from_bt601_video_range_to_rgb[] = {
            1.164383562f,  0.0000000000f,  1.5960267860f, -0.8742022179f, /* RED coeffs */
            1.164383562f, -0.3917622901f, -0.8129676472f,  0.5316678235f, /* GREEN coeffs */
            1.164383562f,  2.0172321430f,  0.0000000000f, -1.0856307890f  /* BLUE coeffs */
        };
        static const float mat_cvt_from_bt709_full_range_to_rgb[] = {
            1.0f,  0.0000000000f, 1.57480000000f, -0.7904878431f, /* RED coeffs */
            1.0f, -0.1873242729f, -0.4681242729f,  0.3290094662f, /* GREEN coeffs */
            1.0f,  1.8556000000f,  0.0000000000f, -0.9314384314f  /* BLUE coeffs */
        };
        static const float mat_cvt_from_bt709_video_range_to_rgb[] = {
            1.164383562f,  0.0000000000f,  1.7927410710f, -0.9729450750f, /* RED coeffs */
            1.164383562f, -0.2132486143f, -0.5329093286f,  0.3014826655f, /* GREEN coeffs */
            1.164383562f,  2.1124017860f,  0.0000000000f, -1.1334022180f  /* BLUE coeffs */
        };
        // clang-format on

        using t = bnb::player_api::pixel_buffer_format;
        switch (from_yuv_format) {
            case t::nv12_bt601_full:
            case t::i420_bt601_full:
                return mat_cvt_from_bt601_full_range_to_rgb;
            case t::nv12_bt601_video:
            case t::i420_bt601_video:
                return mat_cvt_from_bt601_video_range_to_rgb;
            case t::nv12_bt709_full:
            case t::i420_bt709_full:
                return mat_cvt_from_bt709_full_range_to_rgb;
            case t::nv12_bt709_video:
            case t::i420_bt709_video:
                return mat_cvt_from_bt709_video_range_to_rgb;
            default:
                throw std::logic_error("there is no conversion matrix for this type.");
        }
    }

} /* namespace */


namespace
{

    bnb::player_api::pixel_buffer_sptr allocate_pixel_buffer(bnb::player_api::pixel_buffer_format format, int32_t width, int32_t height)
    {
        using pb_t = bnb::player_api::pixel_buffer;
        std::vector<pb_t::plane_data> planes;
        uint8_t* data_raw_ptr;

        using t = bnb::player_api::pixel_buffer_format;
        switch (format) {
            case t::bpc8_rgb:
            case t::bpc8_bgr:
                {
                    int32_t bytes_per_row = (width * 3 + 7) &~ 7;
                    size_t size = bytes_per_row * height;
                    data_raw_ptr = new uint8_t[size];
                    auto plane_data = std::shared_ptr<uint8_t>(data_raw_ptr, [](uint8_t*) { /* DO NOTHING */ });
                    pb_t::plane_data plane(plane_data, size, bytes_per_row);
                    planes.push_back(plane);
                }
                break;
            case t::bpc8_rgba:
            case t::bpc8_bgra:
            case t::bpc8_argb:
                {
                    int32_t bytes_per_row = (width * 4 + 7) &~ 7;
                    size_t size = bytes_per_row * height;
                    data_raw_ptr = new uint8_t[size];
                    auto plane_data = std::shared_ptr<uint8_t>(data_raw_ptr, [](uint8_t*) { /* DO NOTHING */ });
                    pb_t::plane_data plane(plane_data, size, bytes_per_row);
                    planes.push_back(plane);
                }
                break;
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
                {
                    int32_t bytes_per_row = (width + 7) &~ 7;
                    size_t size = bytes_per_row * height * 2;
                    data_raw_ptr = new uint8_t[size];
                    auto y_plane_data = std::shared_ptr<uint8_t>(data_raw_ptr, [](uint8_t*) { /* DO NOTHING */ });
                    auto uv_plane_data = std::shared_ptr<uint8_t>(data_raw_ptr + bytes_per_row * height, [](uint8_t*) { /* DO NOTHING */ });
                    pb_t::plane_data plane_y(y_plane_data, bytes_per_row * height, bytes_per_row);
                    pb_t::plane_data plane_uv(uv_plane_data, bytes_per_row * height / 2, bytes_per_row);
                    planes.push_back(plane_y);
                    planes.push_back(plane_uv);
                }
                break;
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
                {
                    int32_t bytes_per_row = (width + 7) &~ 7;
                    size_t size = bytes_per_row * height * 2;
                    data_raw_ptr = new uint8_t[size];
                    auto y_plane_data = std::shared_ptr<uint8_t>(data_raw_ptr, [](uint8_t*) { /* DO NOTHING */ });
                    auto u_plane_data = std::shared_ptr<uint8_t>(data_raw_ptr + bytes_per_row * height, [](uint8_t*) { /* DO NOTHING */ });
                    auto v_plane_data = std::shared_ptr<uint8_t>(data_raw_ptr + bytes_per_row * height + bytes_per_row * height / 2, [](uint8_t*) { /* DO NOTHING */ });
                    pb_t::plane_data plane_y(y_plane_data, bytes_per_row * height, bytes_per_row);
                    pb_t::plane_data plane_u(u_plane_data, bytes_per_row * height / 2, bytes_per_row);
                    pb_t::plane_data plane_v(v_plane_data, bytes_per_row * height / 2, bytes_per_row);
                    planes.push_back(plane_y);
                    planes.push_back(plane_u);
                    planes.push_back(plane_v);
                }
                break;
        }

        return std::shared_ptr<pb_t>(new pb_t(planes, format, width, height), [](pb_t* pb) {
            delete [] pb->get_base_sptr().get();
            delete pb;
        });
    }

} /* namespace */

namespace bnb::player_api
{

    /* opengl_frame_output::frame_output */
    opengl_frame_output::opengl_frame_output(const pixel_buffer_callback& callback, pixel_buffer_format format)
        : m_pixel_buffer_callback(callback)
        , m_format(format)
    {
        using t = bnb::player_api::pixel_buffer_format;
        switch (m_format) {
            case t::bpc8_rgb:
            case t::bpc8_rgba:
            case t::bpc8_bgr:
            case t::bpc8_bgra:
            case t::bpc8_argb:
                m_format_is_bpc8 = true;
                break;
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
                m_format_is_nv12 = true;
                m_y_plane_convert_coefs = get_conversion_matrix_from_rgb_to_yuv(m_format) + offset_to_y_coeffs;
                m_u_plane_convert_coefs = get_conversion_matrix_from_rgb_to_yuv(m_format) + offset_to_u_coeffs;
                m_v_plane_convert_coefs = get_conversion_matrix_from_rgb_to_yuv(m_format) + offset_to_v_coeffs;
                break;
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
                m_format_is_i420 = true;
                m_y_plane_convert_coefs = get_conversion_matrix_from_rgb_to_yuv(m_format) + offset_to_y_coeffs;
                m_u_plane_convert_coefs = get_conversion_matrix_from_rgb_to_yuv(m_format) + offset_to_u_coeffs;
                m_v_plane_convert_coefs = get_conversion_matrix_from_rgb_to_yuv(m_format) + offset_to_v_coeffs;
                break;
        }
        m_read_image_format = (m_format == t::bpc8_rgb || m_format == t::bpc8_bgr) ? GL_RGB : GL_RGBA;
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
                m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_yuv_source);
                m_shader->use();
                m_uniform_yuv_plane_convert_coefs = m_shader->get_uniform_location("uPlaneConversionCoefs");
                m_uniform_yuv_pixel_step = m_shader->get_uniform_location("uPixelStep");
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

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(render_target->get_output_texture())));
        m_shader->set_uniform_texture(m_uniform_texture, 0);
        m_shader->set_uniform_mat4(m_uniform_matrix, get_orientation_matrix());

        auto width = render_target->get_render_width();
        auto height = render_target->get_render_height();
        auto pb = allocate_pixel_buffer(m_format, width, height);

        if (m_format_is_bpc8) {
            m_renderbuffer->prepare(width, height);

            GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

            GL_CALL(glViewport(0, 0, width, height));
            m_frame_handler->draw_surface();
        
            GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 8));
            GL_CALL(glReadPixels(0, 0, width, height, m_read_image_format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(pb->get_base_sptr().get())));
        } else if (m_format_is_nv12 || m_format_is_i420) {
            auto half_height = height / 2;
            auto full_height = height * 2;
            auto width_div_4 = width / 4;
            auto width_div_8 = width / 8;
            m_renderbuffer->prepare(width_div_4, full_height);

            GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

            GL_CALL(glViewport(0, 0, width_div_4, height));

            float px_step[] = {0.0f, 0.0f};
            switch (m_orientation) {
                case bnb::player_api::orientation::up:
                    px_step[0] = 1.0f / width;
                    break;
                case bnb::player_api::orientation::left:
                    px_step[1] = (m_mirroring ? 1.0f : -1.0f) / width;
                    break;
                case bnb::player_api::orientation::down:
                    px_step[0] = -1.0f / width;
                    break;
                case bnb::player_api::orientation::right:
                    px_step[1] = (m_mirroring ? -1.0f : 1.0f) / width;
                    break;
            }

            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_y_plane_convert_coefs);
            m_shader->set_uniform_vec2(m_uniform_yuv_pixel_step, px_step);
            m_frame_handler->draw_surface();

            px_step[0] *= 2.0f;
            px_step[1] *= 2.0f;

            GL_CALL(glViewport(0, height, width_div_8, half_height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_u_plane_convert_coefs);
            m_shader->set_uniform_vec2(m_uniform_yuv_pixel_step, px_step);
            m_frame_handler->draw_surface();

            GL_CALL(glViewport(0, height + half_height, width_div_8, half_height));
            m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_v_plane_convert_coefs);
            m_shader->set_uniform_vec2(m_uniform_yuv_pixel_step, px_step);
            m_frame_handler->draw_surface();

            GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 8));
            GL_CALL(glReadPixels(0, 0, width_div_4, full_height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(pb->get_base_sptr().get())));
        }

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        opengl_shader_program::unuse();

        m_pixel_buffer_callback(pb);
    }

} /* namespace bnb::player_api */
