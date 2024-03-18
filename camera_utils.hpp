#pragma once

#include <bnb/player_api/pixel_buffer/pixel_buffer.hpp>
#include <bnb/spal/camera/base.hpp>

namespace bnb::example
{


    bnb::player_api::pixel_buffer_sptr full_image_to_pixel_buffer(bnb::full_image_t& image)
    {
        auto width = image.get_format().width;
        auto height = image.get_format().height;
        auto stride = image.get_format().width;
        auto orient = bnb::player_api::orientation::up;
        auto mirror = image.get_format().require_mirroring;

        switch (image.get_format().orientation) {
            case bnb::camera_orientation::deg_0:
                orient = bnb::player_api::orientation::up;
                break;
            case bnb::camera_orientation::deg_90:
                orient = bnb::player_api::orientation::left;
                break;
            case bnb::camera_orientation::deg_180:
                orient = bnb::player_api::orientation::down;
                break;
            case bnb::camera_orientation::deg_270:
                orient = bnb::player_api::orientation::right;
                break;
        }

        if (image.has_data<bnb::yuv_image_t>()) {
            auto yuv_image = image.get_data<bnb::yuv_image_t>();
            auto yuv_format = yuv_image.get_yuv_format();

            bnb::player_api::pixel_buffer_format fmt;
            /* This is a little hack that makes code more linear.
            * But it only works if there are exactly two parameters each enum. If new ones are added, the code needs to be rewritten.
            * used bitmasks:
            * 0b00000100 (0x04) - if the bit is set, then used yuv_format::yuv_nv12   otherwise yuv_format::yuv_i420
            * 0b00000010 (0x02) - if the bit is set, then used color_std::bt601       otherwise color_std::bt709
            * 0b00000001 (0x01) - if the bit is set, then used color_range::full      otherwise color_range::video
            */
            uint8_t tocase =
                (static_cast<uint8_t>(yuv_format.format == bnb::yuv_format::yuv_nv12) << 2) |
                (static_cast<uint8_t>(yuv_format.standard == bnb::color_std::bt601) << 1) |
                (static_cast<uint8_t>(yuv_format.range == bnb::color_range::full) << 0);
            /* 'switch' below handles all existing bitmask variations */
            switch (tocase) {
                case 0b000:
                    fmt = bnb::player_api::pixel_buffer_format::i420_bt709_video;
                    break;
                case 0b001:
                    fmt = bnb::player_api::pixel_buffer_format::i420_bt709_full;
                    break;
                case 0b010:
                    fmt = bnb::player_api::pixel_buffer_format::i420_bt601_video;
                    break;
                case 0b011:
                    fmt = bnb::player_api::pixel_buffer_format::i420_bt601_full;
                    break;
                case 0b100:
                    fmt = bnb::player_api::pixel_buffer_format::nv12_bt709_video;
                    break;
                case 0b101:
                    fmt = bnb::player_api::pixel_buffer_format::nv12_bt709_full;
                    break;
                case 0b110:
                    fmt = bnb::player_api::pixel_buffer_format::nv12_bt601_video;
                    break;
                case 0b111:
                    fmt = bnb::player_api::pixel_buffer_format::nv12_bt601_full;
                    break;
                default:
                    std::cout << "[ERROR] Unknown yuv image format." << std::endl;
                    return nullptr;
            }

            if (tocase & 0b100) { /* nv12 2 planes */
                auto y_data = yuv_image.get_plane<0>();
                auto y_deleter = [y_data](uint8_t*) { /* DO NOTHING */ };
                auto uv_data = yuv_image.get_plane<1>();
                auto uv_deleter = [uv_data](uint8_t*) { /* DO NOTHING */ };

                return std::make_shared<bnb::player_api::pixel_buffer>(
                    y_data.get(), stride,
                    uv_data.get(), stride,
                    width, height, fmt, orient, mirror,
                    y_deleter, uv_deleter);
            } else { /* i420 - 3 planes */
                auto y_data = yuv_image.get_plane<0>();
                auto y_deleter = [y_data](uint8_t*) { /* DO NOTHING */ };
                auto u_data = yuv_image.get_plane<1>();
                auto u_deleter = [u_data](uint8_t*) { /* DO NOTHING */ };
                auto v_data = yuv_image.get_plane<2>();
                auto v_deleter = [v_data](uint8_t*) { /* DO NOTHING */ };

                return std::make_shared<bnb::player_api::pixel_buffer>(
                    y_data.get(), stride,
                    u_data.get(), stride,
                    v_data.get(), stride,
                    width, height, fmt, orient, mirror,
                    y_deleter, u_deleter, v_deleter);
            }
        }

        std::cout << "[ERROR] not yuv image" << std::endl;
        return nullptr;
    }

} /* namespace bnb */
