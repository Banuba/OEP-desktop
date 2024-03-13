#include <bnb/player_api/output/output_base.hpp>

namespace bnb::player_api
{

    /* output_base::set_content_mode */
    void output_base::set_content_mode(content_mode new_content_mode)
    {
        m_content_mode = new_content_mode;
    }

    /* output_base::set_orientation */
    void output_base::set_orientation(orientation orient, bool mirroring)
    {
        m_orientation = orient;
        m_mirroring = mirroring;
    }

    /* output_base::get_orientation_matrix */
    const float* const output_base::get_orientation_matrix()
    {
        // clang-format off
        static const float matrices[] = {
            // mirroring 0, 0 degrees
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 90 degrees
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 180 degrees
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 270 degrees
            0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,

            // mirroring 0, 0 degrees
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 90 degrees
            0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 180 degrees
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            // mirroring 0, 270 degrees
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        // clang-format on
        return matrices + ((static_cast<int32_t>(m_orientation) / 90) + static_cast<int32_t>(m_mirroring) * 4) * 16;
    }

    /* output_base::oriented_frame_size */
    void output_base::oriented_frame_size(const render_target_sptr& rt, int32_t& oriented_width, int32_t& oriented_height) const
    {
        auto swap_sizes = m_orientation == orientation::left || m_orientation == orientation::right;
        if (swap_sizes) {
            oriented_width = rt->get_render_height();
            oriented_height = rt->get_render_width();
        } else {
            oriented_width = rt->get_render_width();
            oriented_height = rt->get_render_height();
        }
    }

    /* output_base::oriented_frame_layout */
    void output_base::oriented_frame_layout(const render_target_sptr& rt, int32_t surface_width, int32_t surface_height, int32_t& left, int32_t& top, int32_t& width, int32_t& height) const
    {
        int32_t oriented_width, oriented_height;
        oriented_frame_size(rt, oriented_width, oriented_height);
        auto width_scale = static_cast<float>(surface_width) / static_cast<float>(oriented_width);
        auto height_scale = static_cast<float>(surface_height) / static_cast<float>(oriented_height);
        auto scale = m_content_mode == content_mode::aspect_fill
                ? (width_scale > height_scale ? width_scale : height_scale) // max
                : (width_scale < height_scale ? width_scale : height_scale); // min
        width = static_cast<int32_t>(static_cast<float>(oriented_width) * scale);
        height = static_cast<int32_t>(static_cast<float>(oriented_height) * scale);
        left = (surface_width - width) / 2;
        top = (surface_height - height) / 2;
    }

} /* namespace bnb::player_api */