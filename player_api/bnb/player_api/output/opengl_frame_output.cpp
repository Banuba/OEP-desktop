#include <bnb/player_api/interfaces/output/opengl_frame_output.hpp>

#include <bnb/player_api/output/orientable_scalable_base.hpp>

#include <bnb/player_api/interfaces/pixel_buffer/pixel_buffer.hpp>
#include <bnb/player_api/utils/yuv_conversion.hpp>
#include <bnb/player_api/utils/common.hpp>
#include <bnb/player_api/opengl/opengl.hpp>
#include <bnb/player_api/opengl/opengl_renderbuffer.hpp>
#include <bnb/player_api/opengl/opengl_shader_program.hpp>
#include <bnb/player_api/opengl/opengl_frame_surface_handler.hpp>
#include <bnb/types/full_image.hpp>

#include <libyuv.h>

namespace
{

    // clang-format off
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
    // clang-format on

    uint8_t* alloc_pixels(size_t size)
    {
        return bnb::player_api::aligned_alloc<64>(size);
    }

    void dealloc_pixels(uint8_t* p)
    {
        bnb::player_api::aligned_dealloc(p);
    }

    bnb::player_api::pixel_buffer_sptr allocate_pixel_buffer(
        bnb::player_api::pixel_buffer_format format,
        bnb::color_std std,
        bnb::color_range rng,
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
            case t::bpc8_bgr: {
                auto stride = bnb::player_api::align_by_8(width * 3);
                auto* data = alloc_pixels(stride * height);
                return pb_t::create(data, stride, width, height, format, orient, mirror, dealloc_pixels);
            } break;
            case t::bpc8_rgba:
            case t::bpc8_bgra:
            case t::bpc8_argb: {
                auto stride = bnb::player_api::align_by_8(width * 4);
                auto* data = alloc_pixels(stride * height);
                return pb_t::create(data, stride, width, height, format, orient, mirror, dealloc_pixels);
            } break;
            case t::nv12: {
                auto stride = bnb::player_api::align_by_32(width);
                auto* data = alloc_pixels(stride * height + stride * bnb::player_api::uv_plane_height(height) + stride);
                auto* uv_data = data + stride * height;
                return pb_t::create(data, stride, uv_data, stride, width, height, format, std, rng, orient, mirror, dealloc_pixels, nullptr);
            } break;
            case t::i420: {
                auto stride = bnb::player_api::align_by_32(width);
                auto* data = alloc_pixels(stride * height + stride * bnb::player_api::uv_plane_height(height));
                auto* u_data = data + stride * height;
                auto* v_data = data + stride * height + bnb::player_api::align_by_16(bnb::player_api::uv_plane_width(width));
                return pb_t::create(data, stride, u_data, stride, v_data, stride, width, height, format, std, rng, orient, mirror, dealloc_pixels, nullptr, nullptr);
            } break;
        }
        return nullptr;
    }



    using namespace bnb::player_api;

    class opengl_frame_output_impl
        : public opengl_frame_output
        , public orientable_scalable_base
    {
    public:
        opengl_frame_output_impl(const pixel_buffer_callback& callback, pixel_buffer_format format)
            : m_pixel_buffer_callback(callback)
            , m_format(format)
            , m_format_is_bpc8(pixel_buffer_format_is_bpc8(format))
            , m_format_is_nv12(pixel_buffer_format_is_nv12(format))
            , m_format_is_i420(pixel_buffer_format_is_i420(format))
            , m_color_standard(bnb::color_std::bt709)
            , m_color_range(bnb::color_range::full)
        {
            validate_not_null(callback);
            using t = bnb::player_api::pixel_buffer_format;
            if (m_format_is_bpc8) {
                m_gl_read_pixels_format = format == t::bpc8_rgb || format == t::bpc8_bgr ? GL_RGB : GL_RGBA;
            } else if (m_format_is_nv12 || m_format_is_i420) {
                m_gl_read_pixels_format = GL_RED;
                set_yuv_format_params(m_color_standard, m_color_range);
            }
        }

        ~opengl_frame_output_impl() = default;

        void set_orientation(orientation orient, bool mirroring = false) override
        {
            update_orientation(orient, mirroring);
        }

        void attach() override
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
                case t::nv12:
                case t::i420:
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

        void detach() override
        {
            m_renderbuffer = nullptr;
            m_frame_handler = nullptr;
            m_shader = nullptr;
        }

        void present(const render_target_sptr& render_target) override
        {
            m_shader->use();
            auto texture = static_cast<uint32_t>(render_target->get_output_texture());
            constexpr uint32_t texture_unit = 0;
            m_shader->set_uniform_texture_unit(m_uniform_texture, texture_unit);
            m_shader->set_uniform_mat4(m_uniform_matrix, get_orientation_matrix(true));

            int32_t width, height;
            oriented_frame_size(render_target, width, height);
            auto renderbuffer_width = width;
            auto renderbuffer_height = height;
            auto pb = allocate_pixel_buffer(m_format, m_color_standard, m_color_range, width, height, m_orientation, m_mirroring);

            if (m_format_is_bpc8) {
                prepare_to_render(renderbuffer_width, renderbuffer_height);
                GL_CALL(glActiveTexture(GL_TEXTURE0 + texture_unit));
                GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
                render(0, 0, width, height);
            } else if (m_format_is_nv12 || m_format_is_i420) {
                auto uv_width = uv_plane_width(width);
                auto uv_height = uv_plane_height(height);
                renderbuffer_width = align_by_32(width);
                renderbuffer_height = height + uv_height + static_cast<int32_t>(m_format_is_nv12);
                auto height_indent_for_nv12 = static_cast<int32_t>(m_format_is_nv12);

                prepare_to_render(renderbuffer_width, renderbuffer_height);
                GL_CALL(glActiveTexture(GL_TEXTURE0 + texture_unit));
                GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
                m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_y_plane_convert_coefs);
                render(0, 0, width, height);
                m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_u_plane_convert_coefs);
                render(0, height + height_indent_for_nv12, uv_width, uv_height);
                m_shader->set_uniform_vec4(m_uniform_yuv_plane_convert_coefs, m_v_plane_convert_coefs);
                render(align_by_16(uv_width), height + height_indent_for_nv12, uv_width, uv_height);
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

            m_pixel_buffer_callback(pb);
        }

        void set_yuv_format_params(bnb::color_std std, bnb::color_range rng) override
        {
            m_color_standard = std;
            m_color_range = rng;
            const auto* const mat = get_conversion_matrix_from_rgb_to_yuv(m_format, std, rng);
            m_y_plane_convert_coefs = mat + yuv_offset_to_y_coeffs;
            m_u_plane_convert_coefs = mat + yuv_offset_to_u_coeffs;
            m_v_plane_convert_coefs = mat + yuv_offset_to_v_coeffs;
        }

    private:
        void prepare_to_render(int32_t framebuffer_width, int32_t framebuffer_height)
        {
            m_renderbuffer->prepare(framebuffer_width, framebuffer_height);
            GL_CALL(glDisable(GL_BLEND));
            GL_CALL(glDisable(GL_CULL_FACE));
            GL_CALL(glDisable(GL_DEPTH_TEST));
        }

        void render(int32_t left, int32_t top, int32_t width, int32_t height)
        {
            GL_CALL(glViewport(left, top, width, height));
            m_frame_handler->draw_surface();
        }

    private:
        pixel_buffer_callback m_pixel_buffer_callback;
        pixel_buffer_format m_format;
        bnb::color_std m_color_standard;
        bnb::color_range m_color_range;
        std::unique_ptr<opengl_renderbuffer> m_renderbuffer;
        std::unique_ptr<opengl_shader_program> m_shader;
        std::unique_ptr<opengl_frame_surface_handler> m_frame_handler;

        const float* m_y_plane_convert_coefs{nullptr};
        const float* m_u_plane_convert_coefs{nullptr};
        const float* m_v_plane_convert_coefs{nullptr};

        bool m_format_is_bpc8{false};
        bool m_format_is_nv12{false};
        bool m_format_is_i420{false};

        uint32_t m_gl_read_pixels_format{0};

        int32_t m_uniform_texture{0};
        int32_t m_uniform_matrix{0};
        int32_t m_uniform_yuv_plane_convert_coefs{0};
    }; // class opengl_frame_output_impl

} // namespace

using t = bnb::player_api::opengl_frame_output;
std::shared_ptr<t> t::create(const t::pixel_buffer_callback& callback, bnb::player_api::pixel_buffer_format format)
{
    return std::make_shared<opengl_frame_output_impl>(callback, format);
}
