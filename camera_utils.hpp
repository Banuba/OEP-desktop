#pragma once

#include <bnb/player_api/interfaces/pixel_buffer/pixel_buffer.hpp>
#include <bnb/spal/camera/base.hpp>

#include <exception>

namespace bnb::example
{

    bnb::player_api::pixel_buffer_sptr full_image_to_pixel_buffer(bnb::full_image_t& image)
    {
        auto width = image.get_format().width;
        auto height = image.get_format().height;
        auto stride = image.get_format().width;
        auto orient = bnb::player_api::camera_orientation_to_orientation(image.get_format().orientation);
        auto mirror = image.get_format().require_mirroring;

        if (image.has_data<bnb::yuv_image_t>()) {
            auto yuv_image = image.get_data<bnb::yuv_image_t>();
            auto format = yuv_image.get_yuv_format().format == bnb::yuv_format::yuv_nv12
                ? bnb::player_api::pixel_buffer_format::nv12
                : bnb::player_api::pixel_buffer_format::i420;
            auto std = yuv_image.get_yuv_format().standard;
            auto rng = yuv_image.get_yuv_format().range;
            auto data_first_plane = yuv_image.get_plane<0>();
            auto deleter_first_plane = [data_first_plane](uint8_t*) { /* DO NOTHING */ };
            auto data_second_plane = yuv_image.get_plane<1>();
            auto deleter_second_plane = [data_second_plane](uint8_t*) { /* DO NOTHING */ };

            if (bnb::player_api::pixel_buffer_format_is_nv12(format)) { // i420 - 2 planes
                return bnb::player_api::pixel_buffer::create(
                    data_first_plane.get(), stride,
                    data_second_plane.get(), stride,
                    width, height, format, std, rng, orient, mirror,
                    deleter_first_plane, deleter_second_plane);
            } else { // i420 - 3 planes
                auto data_third_plane = yuv_image.get_plane<2>();
                auto deleter_third_plane = [data_third_plane](uint8_t*) { /* DO NOTHING */ };

                return bnb::player_api::pixel_buffer::create(
                    data_first_plane.get(), stride,
                    data_second_plane.get(), stride,
                    data_third_plane.get(), stride,
                    width, height, format, std, rng, orient, mirror,
                    deleter_first_plane, deleter_second_plane, deleter_third_plane);
            }
        } else if (image.has_data<bnb::bpc8_image_t>()) {
            auto bpc8_image = image.get_data<bnb::bpc8_image_t>();
            auto format = bnb::player_api::pixel_buffer_format::bpc8_rgb;
            switch (bpc8_image.get_pixel_format()) {
                case bnb::interfaces::pixel_format::rgb:
                    format = bnb::player_api::pixel_buffer_format::bpc8_rgb;
                    break;
                case bnb::interfaces::pixel_format::bgr:
                    format = bnb::player_api::pixel_buffer_format::bpc8_bgr;
                    break;
                case bnb::interfaces::pixel_format::bgra:
                    format = bnb::player_api::pixel_buffer_format::bpc8_bgra;
                    break;
                case bnb::interfaces::pixel_format::rgba:
                    format = bnb::player_api::pixel_buffer_format::bpc8_rgba;
                    break;
                case bnb::interfaces::pixel_format::argb:
                    format = bnb::player_api::pixel_buffer_format::bpc8_argb;
                    break;
                default:
                    throw std::logic_error("Undefined pixel format.");
            }

            auto* data = bpc8_image.get_data(); // TODO: check whether data needs to be copied
            auto deleter = [](uint8_t*) { /* DO NOTHING */ };

            return bnb::player_api::pixel_buffer::create(
                data, stride,
                width, height, format, orient, mirror,
                deleter);
        }

        throw std::runtime_error("Image contains no data.");
    }

} /* namespace bnb */
