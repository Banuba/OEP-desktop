#include <bnb/player_api/input/stream_input.hpp>

#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/player_api/types/pixel_buffer_format.hpp>
#include <bnb/types/interfaces/all.hpp>

namespace
{

    bnb::image_format make_bnb_image_format(const bnb::player_api::pixel_buffer_sptr& image, bnb::player_api::orientation orient, bool require_mirroring)
    {
        bnb::camera_orientation camera_orient {bnb::camera_orientation::deg_0};

        using ns = bnb::player_api::orientation;
        switch (orient) {
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

        return {static_cast<uint32_t>(image->get_width()), static_cast<uint32_t>(image->get_height()), camera_orient, require_mirroring, 0, std::nullopt};
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

    /* stream_input::stream_input */
    stream_input::stream_input(processor_type processor, bool offline_mode, bool future_interpolate, bool future_filter)
    {
        auto config = bnb::interfaces::processor_configuration::create();
        config->set_use_offline_mode(offline_mode);
        config->set_use_future_interpolate(future_interpolate);
        config->set_use_future_filter(future_filter);
        switch (processor) {
        case processor_type::realtime:
            m_frame_processor = bnb::interfaces::frame_processor::create_realtime_processor(bnb::interfaces::realtime_processor_mode::async_when_effect_loaded, config);
            break;
        case processor_type::photo:
            m_frame_processor = bnb::interfaces::frame_processor::create_photo_processor(config);
            break;
        case processor_type::video:
            m_frame_processor = bnb::interfaces::frame_processor::create_video_processor(config);
            break;
        }
    }

    /* stream_input::push */
    void stream_input::push(const frame_data_sptr& fd)
    {
        m_frame_processor->push(fd);
    }

    /* stream_input::push */
    void stream_input::push(const full_image_t& image, uint64_t timestamp_us)
    {
        m_timestamp = timestamp_us;
        auto fd = bnb::interfaces::frame_data::create();
        fd->add_full_img(image);
        fd->add_timestamp_us(timestamp_us);
        push(fd);
    }
    
    /* stream_input::push */
    void stream_input::push(const pixel_buffer_sptr& image, uint64_t timestamp_us, orientation orient, bool require_mirroring)
    {
        using ns = bnb::player_api::pixel_buffer_format;
        auto bnb_image_format = make_bnb_image_format(image, orient, require_mirroring);
        switch (image->get_format()) {
            case ns::bpc8_rgb:
            case ns::bpc8_bgr:
            case ns::bpc8_bgra:
            case ns::bpc8_argb:
                push(
                    full_image_t(bpc8_image_t(
                        color_plane(image->get_base_ptr()),
                        make_bnb_pixel_format(image),
                        bnb_image_format)),
                    timestamp_us);
                break;
            case ns::nv12_bt601_full:
            case ns::nv12_bt601_video:
            case ns::nv12_bt709_full:
            case ns::nv12_bt709_video:
                push(
                    full_image_t(yuv_image_t(
                        color_plane(image->get_base_ptr_of_plane(0)),
                        color_plane(image->get_base_ptr_of_plane(1)),
                        bnb_image_format,
                        make_bnb_yuv_format(image))),
                    timestamp_us);
                break;
            case ns::i420_bt601_full:
            case ns::i420_bt601_video:
            case ns::i420_bt709_full:
            case ns::i420_bt709_video:
                push(
                    full_image_t(yuv_image_t(
                        color_plane(image->get_base_ptr_of_plane(0)),
                        color_plane(image->get_base_ptr_of_plane(1)),
                        color_plane(image->get_base_ptr_of_plane(2)),
                        bnb_image_format,
                        make_bnb_yuv_format(image))),
                    timestamp_us);
                break;
            default:
                break;
        }
    }

    /* stream_input::get_frame_processor */
    frame_processor_sptr stream_input::get_frame_processor() const noexcept
    {
        return m_frame_processor;
    }

    /* stream_input::get_frame_time_us */
    uint64_t stream_input::get_frame_time_us() const noexcept
    {
        return m_timestamp;
    }

} /* namespace bnb::player_api */
