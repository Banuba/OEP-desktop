#include <bnb/player_api/input/input_pixels_base.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace
{

    bnb::image_format make_bnb_image_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        auto orient = bnb::player_api::orientation_to_camera_orientation(image->get_orientation());
        return bnb::image_format(image->get_width(), image->get_height(), orient, image->get_mirroring(), 0);
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
            case ns::bpc8_bgr:
            case ns::bpc8_bgra:
            case ns::bpc8_argb:
                push(
                    full_image_t(bpc8_image_t(
                        st(image->get_base_ptr(), [image](t*) {}),
                        pixel_buffer_format_to_pixel_format(image->get_format()),
                        make_bnb_image_format(image)
                    )),
                    timestamp_us
                );
                break;
            case ns::nv12_bt601_full:
            case ns::nv12_bt601_video:
            case ns::nv12_bt709_full:
            case ns::nv12_bt709_video:
                push(
                    full_image_t(yuv_image_t(
                        st(image->get_base_ptr_of_plane(0), [image](t*) {}),
                        st(image->get_base_ptr_of_plane(1), [image](t*) {}),
                        make_bnb_image_format(image),
                        pixel_buffer_format_to_yuv_format(image->get_format())
                    )),
                    timestamp_us
                );
                break;
            case ns::i420_bt601_full:
            case ns::i420_bt601_video:
            case ns::i420_bt709_full:
            case ns::i420_bt709_video:
                push(
                    full_image_t(yuv_image_t(
                        st(image->get_base_ptr_of_plane(0), [image](t*) {}),
                        st(image->get_base_ptr_of_plane(1), [image](t*) {}),
                        st(image->get_base_ptr_of_plane(2), [image](t*) {}),
                        make_bnb_image_format(image),
                        pixel_buffer_format_to_yuv_format(image->get_format())
                    )),
                    timestamp_us
                );
                break;
            default:
                break;
        }
    }

} // namespace bnb::player_api
