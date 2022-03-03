#include "camera_utils.hpp"

#include <interfaces/image_format.hpp>

#include <iostream>

namespace bnb
{

    pixel_buffer_sptr camera_utils::full_image_to_pixel_buffer(bnb::full_image_t &image)
    {
        int32_t width = image.get_format().width;
        int32_t height = image.get_format().height;
        int32_t stride = image.get_format().width;

        if (image.has_data<bnb::yuv_image_t>()) {
            auto yuv_image = image.get_data<bnb::yuv_image_t>();
            auto yuv_format = yuv_image.get_yuv_format();

            bnb::oep::interfaces::image_format outfmt;
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
                    outfmt = bnb::oep::interfaces::image_format::i420_bt709_video;
                    break;
                case 0b001:
                    outfmt = bnb::oep::interfaces::image_format::i420_bt709_full;
                    break;
                case 0b010:
                    outfmt = bnb::oep::interfaces::image_format::i420_bt601_video;
                    break;
                case 0b011:
                    outfmt = bnb::oep::interfaces::image_format::i420_bt601_full;
                    break;
                case 0b100:
                    outfmt = bnb::oep::interfaces::image_format::nv12_bt709_video;
                    break;
                case 0b101:
                    outfmt = bnb::oep::interfaces::image_format::nv12_bt709_full;
                    break;
                case 0b110:
                    outfmt = bnb::oep::interfaces::image_format::nv12_bt601_video;
                    break;
                case 0b111:
                    outfmt = bnb::oep::interfaces::image_format::nv12_bt601_full;
                    break;
                default:
                    std::cout << "[ERROR] Unknown yuv image format" << std::endl;
                    return nullptr;
            }

            if (tocase & 0b100) { /* nv12 2 planes */
                std::vector<bnb::oep::interfaces::pixel_buffer::plane_data> planes {
                    {yuv_image.get_plane<0>(), 0, stride},
                    {yuv_image.get_plane<1>(), 0, stride}
                };
                return bnb::oep::interfaces::pixel_buffer::create(planes, outfmt, width, height);
            } else { /* i420 - 3 planes */
                std::vector<bnb::oep::interfaces::pixel_buffer::plane_data> planes {
                    {yuv_image.get_plane<0>(), 0, stride},
                    {yuv_image.get_plane<1>(), 0, stride},
                    {yuv_image.get_plane<2>(), 0, stride}
                };
                return bnb::oep::interfaces::pixel_buffer::create(planes, outfmt, width, height);
            }
            return nullptr;
        }

        std::cout << "[ERROR] not yuv image" << std::endl;
        return nullptr;
    }

} /* namespace bnb */
