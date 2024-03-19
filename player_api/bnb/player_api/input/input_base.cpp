#include <bnb/player_api/input/input_base.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace
{

    bnb::image_format make_bnb_image_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        bnb::camera_orientation camera_orient {bnb::camera_orientation::deg_0};

        using ns = bnb::player_api::orientation;
        switch (image->get_orientation()) {
            case ns::up:
                break;
            case ns::left:
                camera_orient = bnb::camera_orientation::deg_90;
                break;
            case ns::down:
                camera_orient = bnb::camera_orientation::deg_180;
                break;
            case ns::right:
                camera_orient = bnb::camera_orientation::deg_270;
                break;
        }

        return {static_cast<uint32_t>(image->get_width()), static_cast<uint32_t>(image->get_height()), camera_orient, image->get_mirroring(), 0, std::nullopt};
    }

    bnb::yuv_format_t make_bnb_yuv_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        bnb::yuv_format format {bnb::yuv_format::yuv_nv12};  /* i.e. NV12 or I420 */
        bnb::color_std standard {bnb::color_std::bt601}; /* i.e. BT.601 or BT.709 */
        bnb::color_range range {bnb::color_range::full}; /* i.e. "full" or "video" */

        using ns = bnb::player_api::pixel_buffer_format;
        switch (image->get_format()) {
            case ns::nv12_bt601_full:
                break;
            case ns::nv12_bt601_video:
                range = bnb::color_range::video;
                break;
            case ns::nv12_bt709_full:
                standard = bnb::color_std::bt709;
                break;
            case ns::nv12_bt709_video:
                range = bnb::color_range::video;
                standard = bnb::color_std::bt709;
                break;
            case ns::i420_bt601_full:
                format = bnb::yuv_format::yuv_i420;
                break;
            case ns::i420_bt601_video:
                range = bnb::color_range::video;
                format = bnb::yuv_format::yuv_i420;
                break;
            case ns::i420_bt709_full:
                format = bnb::yuv_format::yuv_i420;
                standard = bnb::color_std::bt709;
                break;
            case ns::i420_bt709_video:
                range = bnb::color_range::video;
                format = bnb::yuv_format::yuv_i420;
                standard = bnb::color_std::bt709;
                break;
            default:
                break;
        }

        return {range, standard, format};
    }

    bnb::interfaces::pixel_format make_bnb_pixel_format(const bnb::player_api::pixel_buffer_sptr& image)
    {
        bnb::interfaces::pixel_format fmt {bnb::interfaces::pixel_format::rgb};

        using ns = bnb::player_api::pixel_buffer_format;
        switch (image->get_format()) {
            case ns::bpc8_rgb:
                break;
            case ns::bpc8_bgr:
                fmt = bnb::interfaces::pixel_format::bgr;
                break;
            case ns::bpc8_rgba:
                fmt = bnb::interfaces::pixel_format::rgba;
                break;
            case ns::bpc8_bgra:
                fmt = bnb::interfaces::pixel_format::bgra;
                break;
            case ns::bpc8_argb:
                fmt = bnb::interfaces::pixel_format::argb;
                break;
            default:
                break;
        }
        return fmt;
    }

} /* namespace */

namespace bnb::player_api
{

    /* input_base::get_frame_processor */
    frame_processor_sptr input_base::get_frame_processor() const noexcept
    {
        return m_frame_processor;
    }

    /* input_base::get_frame_time_us */
    uint64_t input_base::get_frame_time_us() const noexcept
    {
        return m_timestamp;
    }

    /* input_base::push_frame_data */
    void input_base::push_frame_data(const frame_data_sptr& fd)
    {
        m_frame_processor->push(fd);
    }

    /* input_base::push_full_image */
    void input_base::push_full_image(const full_image_t& image, uint64_t timestamp_us)
    {
        m_timestamp = timestamp_us;
        auto fd = bnb::interfaces::frame_data::create();
        fd->add_full_img(image);
        fd->add_timestamp_us(timestamp_us);
        push_frame_data(fd);
    }

    /* input_base::push_pixel_buffer */
    void input_base::push_pixel_buffer(const pixel_buffer_sptr& image, uint64_t timestamp_us)
    {
        using ns = bnb::player_api::pixel_buffer_format;
        switch (image->get_format()) {
            case ns::bpc8_rgb:
            case ns::bpc8_bgr:
            case ns::bpc8_bgra:
            case ns::bpc8_argb:
                push_full_image(
                    full_image_t(bpc8_image_t(
                        std::shared_ptr<bnb::color_plane_data_t>(image->get_base_ptr(), [image](bnb::color_plane_data_t*) {}),
                        make_bnb_pixel_format(image),
                        make_bnb_image_format(image))),
                    timestamp_us);
                break;
            case ns::nv12_bt601_full:
            case ns::nv12_bt601_video:
            case ns::nv12_bt709_full:
            case ns::nv12_bt709_video:
                push_full_image(
                    full_image_t(yuv_image_t(
                        std::shared_ptr<bnb::color_plane_data_t>(image->get_base_ptr_of_plane(0), [image](bnb::color_plane_data_t*) {}),
                        std::shared_ptr<bnb::color_plane_data_t>(image->get_base_ptr_of_plane(1), [image](bnb::color_plane_data_t*) {}),
                        make_bnb_image_format(image),
                        make_bnb_yuv_format(image))),
                    timestamp_us);
                break;
            case ns::i420_bt601_full:
            case ns::i420_bt601_video:
            case ns::i420_bt709_full:
            case ns::i420_bt709_video:
                push_full_image(
                    full_image_t(yuv_image_t(
                        std::shared_ptr<bnb::color_plane_data_t>(image->get_base_ptr_of_plane(0), [image](bnb::color_plane_data_t*) {}),
                        std::shared_ptr<bnb::color_plane_data_t>(image->get_base_ptr_of_plane(1), [image](bnb::color_plane_data_t*) {}),
                        std::shared_ptr<bnb::color_plane_data_t>(image->get_base_ptr_of_plane(2), [image](bnb::color_plane_data_t*) {}),
                        make_bnb_image_format(image),
                        make_bnb_yuv_format(image))),
                    timestamp_us);
                break;
            default:
                break;
        }
    }

} /* namespace bnb::player_api */
