#pragma once

#include <bnb/player_api/pixel_buffer/pixel_buffer.hpp>
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
            auto format = bnb::player_api::yuv_format_to_pixel_buffer_format(yuv_image.get_yuv_format());

            auto data_first_plane = yuv_image.get_plane<0>();
            auto deleter_first_plane = [data_first_plane](uint8_t*) { /* DO NOTHING */ };
            auto data_second_plane = yuv_image.get_plane<1>();
            auto deleter_second_plane = [data_second_plane](uint8_t*) { /* DO NOTHING */ };

            if (bnb::player_api::pixel_buffer_format_is_nv12(format)) { // i420 - 2 planes
                return std::make_shared<bnb::player_api::pixel_buffer>(
                    data_first_plane.get(), stride,
                    data_second_plane.get(), stride,
                    width, height, format, orient, mirror,
                    deleter_first_plane, deleter_second_plane);
            } else { // i420 - 3 planes
                auto data_third_plane = yuv_image.get_plane<2>();
                auto deleter_third_plane = [data_third_plane](uint8_t*) { /* DO NOTHING */ };

                return std::make_shared<bnb::player_api::pixel_buffer>(
                    data_first_plane.get(), stride,
                    data_second_plane.get(), stride,
                    data_third_plane.get(), stride,
                    width, height, format, orient, mirror,
                    deleter_first_plane, deleter_second_plane, deleter_third_plane);
            }
        } else if (image.has_data<bnb::bpc8_image_t>()) {
            auto bpc8_image = image.get_data<bnb::bpc8_image_t>();
            auto format = bnb::player_api::pixel_format_to_pixel_buffer_format(bpc8_image.get_pixel_format());

            auto* data = bpc8_image.get_data(); // TODO: check whether data needs to be copied
            auto deleter = [](uint8_t*) { /* DO NOTHING */ };

            return std::make_shared<bnb::player_api::pixel_buffer>(
                data, stride,
                width, height, format, orient, mirror,
                deleter);
        }

        throw std::runtime_error("Image contains no data.");
    }

} /* namespace bnb */
