#include <bnb/player_api/input/input_pixels_base.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>
#include <bnb/types/full_image.hpp>

namespace
{

    bnb::image_format make_bnb_image_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        auto orient = bnb::player_api::orientation_to_camera_orientation(image->get_orientation());
        return bnb::image_format(image->get_width(), image->get_height(), orient, image->get_mirroring(), 0);
    }

    bnb::yuv_format_t pixel_buffer_format_to_yuv_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        auto format = image->get_format() == bnb::player_api::pixel_buffer_format::i420 ? bnb::yuv_format::yuv_i420 : bnb::yuv_format::yuv_nv12;
        return {image->get_color_range(), image->get_color_standard(), format};
    }

    bnb::interfaces::pixel_format pixel_buffer_format_to_pixel_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        using t = bnb::player_api::pixel_buffer_format;
        switch (image->get_format()) {
            case t::bpc8_rgb:
                return bnb::interfaces::pixel_format::rgb;
            case t::bpc8_rgba:
                return bnb::interfaces::pixel_format::rgba;
            case t::bpc8_bgr:
                return bnb::interfaces::pixel_format::bgr;
            case t::bpc8_bgra:
                return bnb::interfaces::pixel_format::bgra;
            case t::bpc8_argb:
                return bnb::interfaces::pixel_format::argb;
            default:
                throw std::runtime_error("Incorrect pixel buffer format, only bpc8 format available.");
        }
    }

} // namespace

namespace bnb::player_api
{

    /* input_pixels_base::get_frame_processor */
    frame_processor_sptr input_pixels_base::get_frame_processor() const noexcept
    {
        return m_frame_processor;
    }

    /* input_pixels_base::get_frame_time_us */
    uint64_t input_pixels_base::get_frame_time_us() const noexcept
    {
        return m_timestamp;
    }

    /* input_pixels_base::push_frame_data */
    void input_pixels_base::push(const frame_data_sptr& fd)
    {
        m_frame_processor->push(fd);
    }

    /* input_pixels_base::push_full_image */
    void input_pixels_base::push(const full_image_t& image, uint64_t timestamp_us)
    {
        m_timestamp = timestamp_us;
        auto fd = bnb::interfaces::frame_data::create();
        fd->add_full_img(image);
        fd->add_timestamp_us(timestamp_us);
        push(fd);
    }

    /* input_pixels_base::push_pixel_buffer */
    void input_pixels_base::push(const pixel_buffer_sptr& image, uint64_t timestamp_us)
    {
        using ns = bnb::player_api::pixel_buffer_format;
        using t = bnb::color_plane_data_t;
        using st = std::shared_ptr<t>;
        switch (image->get_format()) {
            case ns::bpc8_rgb:
            case ns::bpc8_rgba:
            case ns::bpc8_bgr:
            case ns::bpc8_bgra:
            case ns::bpc8_argb:
                push(
                    full_image_t(bpc8_image_t(
                        st(image->get_base_ptr(), [image](t*) {}),
                        pixel_buffer_format_to_pixel_format(image),
                        make_bnb_image_format(image)
                    )),
                    timestamp_us
                );
                break;
            case ns::nv12:
                push(
                    full_image_t(yuv_image_t(
                        st(image->get_base_ptr_of_plane(0), [image](t*) {}),
                        st(image->get_base_ptr_of_plane(1), [image](t*) {}),
                        make_bnb_image_format(image),
                        pixel_buffer_format_to_yuv_format(image)
                    )),
                    timestamp_us
                );
                break;
            case ns::i420:
                push(
                    full_image_t(yuv_image_t(
                        st(image->get_base_ptr_of_plane(0), [image](t*) {}),
                        st(image->get_base_ptr_of_plane(1), [image](t*) {}),
                        st(image->get_base_ptr_of_plane(2), [image](t*) {}),
                        make_bnb_image_format(image),
                        pixel_buffer_format_to_yuv_format(image)
                    )),
                    timestamp_us
                );
                break;
        }
    }

} // namespace bnb::player_api
