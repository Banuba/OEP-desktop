#include <bnb/player_api/utils/yuv_conversion.hpp>

#include <exception>

namespace bnb::player_api
{

    const float* const get_conversion_matrix_from_rgb_to_yuv(bnb::player_api::pixel_buffer_format to_yuv_format)
    {
        // The base values for calculating coefficients were taken from the corresponding standard.
        // See link to WIKI: https://en.wikipedia.org/wiki/YUV
        // clang-format off
        static const float mat_cvt_from_rgb_to_bt601_video_range[] = {
             0.256788f,  0.504129f,  0.0979059f, 0.0627451f, // Y plane coefficients
            -0.148223f, -0.290993f,  0.439216f,  0.501961f,  // U plane coefficients
             0.439216f, -0.367788f, -0.0714274f, 0.501961f,  // V plane coefficients
        };
        static const float mat_cvt_from_rgb_to_bt601_full_range[] = {
             0.2990000f,     0.587000f,      0.1140000f,     0.000000f, // Y plane coefficients
            -0.1687358916f, -0.3312641084f,  0.50f,          0.50f,     // U plane coefficients
             0.50f,         -0.4186875892f, -0.08131241084f, 0.50f,     // V plane coefficients
        };
        static const float mat_cvt_from_rgb_to_bt709_video_range[] = {
             0.1825858824f,  0.6142305882f,  0.06200705882f, 0.06274509804f, // Y plane coefficients
            -0.1006437324f, -0.3385719539f,  0.4392156863f,  0.5019607843f,  // U plane coefficients
             0.4392156863f, -0.3989421626f, -0.04027352368f, 0.5019607843f,  // V plane coefficients
        };
        static const float mat_cvt_from_rgb_to_bt709_full_range[] = {
             0.212600000f,   0.71520000f,    0.07220000f,    0.00000000f,   // Y plane coefficients
            -0.1145721061f, -0.3854278939f,  0.5f,           0.5019607843f, // U plane coefficients
             0.5f,          -0.4541529083f, -0.04584709169f, 0.5019607843f, // V plane coefficients
        };
        // clang-format on

        using t = bnb::player_api::pixel_buffer_format;
        switch (to_yuv_format) {
            case t::nv12_bt601_full:
            case t::i420_bt601_full:
                return mat_cvt_from_rgb_to_bt601_full_range;
            case t::nv12_bt601_video:
            case t::i420_bt601_video:
                return mat_cvt_from_rgb_to_bt601_video_range;
            case t::nv12_bt709_full:
            case t::i420_bt709_full:
                return mat_cvt_from_rgb_to_bt709_full_range;
            case t::nv12_bt709_video:
            case t::i420_bt709_video:
                return mat_cvt_from_rgb_to_bt709_video_range;
            default:
                throw std::logic_error("there is no conversion matrix for this type.");
        }
    }

    const float* const get_conversion_matrix_from_yuv_to_rgb(bnb::player_api::pixel_buffer_format from_yuv_format)
    {
        // The base values for calculating coefficients were taken from the corresponding standard.
        // See link to WIKI: https://en.wikipedia.org/wiki/YUV
        // clang-format off
        static const float mat_cvt_from_bt601_full_range_to_rgb[] = {
            1.0f,  0.0000000000f,  1.4020000000f, -0.7037490196f, // RED coeffs
            1.0f, -0.3441362862f, -0.7141362862f,  0.5312113305f, // GREEN coeffs
            1.0f,  1.7720000000f,  0.0000000000f, -0.8894745098f, // BLUE coeffs
        };
        static const float mat_cvt_from_bt601_video_range_to_rgb[] = {
            1.164383562f,  0.0000000000f,  1.5960267860f, -0.8742022179f, // RED coeffs
            1.164383562f, -0.3917622901f, -0.8129676472f,  0.5316678235f, // GREEN coeffs
            1.164383562f,  2.0172321430f,  0.0000000000f, -1.0856307890f, // BLUE coeffs
        };
        static const float mat_cvt_from_bt709_full_range_to_rgb[] = {
            1.0f,  0.0000000000f, 1.57480000000f, -0.7904878431f, // RED coeffs
            1.0f, -0.1873242729f, -0.4681242729f,  0.3290094662f, // GREEN coeffs
            1.0f,  1.8556000000f,  0.0000000000f, -0.9314384314f, // BLUE coeffs
        };
        static const float mat_cvt_from_bt709_video_range_to_rgb[] = {
            1.164383562f,  0.0000000000f,  1.7927410710f, -0.9729450750f, // RED coeffs
            1.164383562f, -0.2132486143f, -0.5329093286f,  0.3014826655f, // GREEN coeffs
            1.164383562f,  2.1124017860f,  0.0000000000f, -1.1334022180f, // BLUE coeffs
        };
        // clang-format on

        using t = bnb::player_api::pixel_buffer_format;
        switch (from_yuv_format) {
            case t::nv12_bt601_full:
            case t::i420_bt601_full:
                return mat_cvt_from_bt601_full_range_to_rgb;
            case t::nv12_bt601_video:
            case t::i420_bt601_video:
                return mat_cvt_from_bt601_video_range_to_rgb;
            case t::nv12_bt709_full:
            case t::i420_bt709_full:
                return mat_cvt_from_bt709_full_range_to_rgb;
            case t::nv12_bt709_video:
            case t::i420_bt709_video:
                return mat_cvt_from_bt709_video_range_to_rgb;
            default:
                throw std::logic_error("there is no conversion matrix for this type.");
        }
    }

} // namespace bnb::player_api
