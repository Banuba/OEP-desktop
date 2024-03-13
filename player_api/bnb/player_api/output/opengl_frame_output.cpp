#include <bnb/player_api/output/opengl_frame_output.hpp>

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


namespace
{


    bnb::player_api::pixel_buffer_sptr allocate_pixel_buffer(bnb::player_api::pixel_buffer_format format, int32_t width, int32_t height)
    {
        int32_t bytes_per_row = 0;
        size_t size = 0;
        std::vector<bnb::player_api::pixel_buffer::plane_data> planes;

        using t = bnb::player_api::pixel_buffer_format;
        switch (format) {
            case t::bpc8_rgb:
            case t::bpc8_bgr:
                bytes_per_row = width * 3 + 7 &~ 7;
                size = bytes_per_row * height;
            case t::bpc8_rgba:
            case t::bpc8_bgra:
            case t::bpc8_argb:
                {
                    if (bytes_per_row == 0) {
                        bytes_per_row = width * 4 + 7 &~ 7;
                        size = bytes_per_row * height;
                    }
                    auto raw_data_ptr = std::shared_ptr<uint8_t>(new uint8_t[size], [](uint8_t*) { /* DO NOTHING */ });
                    bnb::player_api::pixel_buffer::plane_data plane {raw_data_ptr, size, bytes_per_row};
                    planes.emplace_back(plane);
                }
                break;
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
                {
                    bytes_per_row = width + 7 &~ 7;
                    size = bytes_per_row * height + bytes_per_row * height / 2;
                    auto raw_data_ptr = std::shared_ptr<uint8_t>(new uint8_t[size], [](uint8_t*) { /* DO NOTHING */ });
                }
//                raw_data_ptr = std::shared_ptr<uint8_t>(new uint8_t[size], [](uint8_t*) { /* DO NOTHING */ });
                break;
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
                bytes_per_row = width + 7 &~ 7;
                size = bytes_per_row * height + bytes_per_row * height / 2;
                break;
        }

        auto* raw_pb = new bnb::player_api::pixel_buffer(planes, format, width, height);
        auto output = std::shared_ptr<bnb::player_api::pixel_buffer>(raw_pb, [](bnb::player_api::pixel_buffer* pb) {
            delete [] pb->get_base_sptr().get();
            delete pb;
        });
    }

} /* namespace */

namespace bnb::player_api
{

    /* frame_output::frame_output */
    opengl_frame_output::opengl_frame_output(const pixel_buffer_callback& callback, pixel_buffer_format format)
        : m_pixel_buffer_callback(callback)
        , m_format(pixel_buffer_format::bpc8_rgba)
    {
    }

    /* frame_output::present */
    void opengl_frame_output::present(const render_target_sptr& render_target)
    {
        auto swap_sizes = m_orientation == orientation::left || m_orientation == orientation::right;
        auto frame_width = swap_sizes ? render_target->get_render_height() : render_target->get_render_width();
        auto frame_height = swap_sizes ? render_target->get_render_width() : render_target->get_render_height();
        prepare_rgba_data_storage(frame_width, frame_height);
//        render_target->read_rgba_frame(m_rgba_frame, get_static_rotation_mat4(m_orientation, m_mirroring));

        if (m_format == pixel_buffer_format::bpc8_rgba) {
            //return m_rgba_frame;
        }

        m_pixel_buffer_callback(convert());
    }

    /* frame_output::prepare_rgba_data_storage */
    void opengl_frame_output::prepare_rgba_data_storage(int32_t width, int32_t height)
    {
//        if (m_rgba_frame == nullptr || m_rgba_frame->get_width() != width || m_rgba_frame->get_height() != height) {
//            auto bytes_per_row = static_cast<int32_t>((width * 4 + 7) &~ 7);
//            auto size = static_cast<size_t>(bytes_per_row * height);
//            auto new_data_ptr = std::shared_ptr<uint8_t>(new uint8_t[size], std::default_delete<uint8_t>());
//            pixel_buffer::plane_data pd {new_data_ptr, size, bytes_per_row};
//            std::vector<pixel_buffer::plane_data> planes {pd};
//            m_rgba_frame = std::make_shared<pixel_buffer>(planes, pixel_buffer_format::bpc8_rgba, width, height);
//        }
    }

    /* frame_output::convert */
    pixel_buffer_sptr opengl_frame_output::convert()
    {
        pixel_buffer_sptr output;
        int32_t bytes_per_row = 0;
        size_t size = 0;
        std::shared_ptr<uint8_t> new_data_ptr;

        using t = bnb::player_api::pixel_buffer_format;
        switch (m_format) {
            case t::bpc8_rgb:
                //bytes_per_row = ;
                break;
            case t::bpc8_bgr:
                break;
            case t::bpc8_rgba:
                break;
            case t::bpc8_bgra:
                break;
            case t::bpc8_argb:
                break;
            case t::nv12_bt601_full:
                break;
            case t::nv12_bt601_video:
                break;
            case t::nv12_bt709_full:
                break;
            case t::nv12_bt709_video:
                break;
            case t::i420_bt601_full:
                break;
            case t::i420_bt601_video:
                break;
            case t::i420_bt709_full:
                break;
            case t::i420_bt709_video:
                break;
        }
    }

} /* namespace bnb::player_api */
