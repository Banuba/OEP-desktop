#include <bnb/player_api/output/window_output.hpp>

#include <math.h>

namespace
{
/**
 * Calculates the coordinates of a rectangular drawing area according to the selected logic.
 */
void calculate_frame_layout(
        int32_t surface_width,
        int32_t surface_height,
        int32_t frame_width,
        int32_t frame_height,
        bnb::player_api::interfaces::output::content_mode content_mode,
        int32_t& left,
        int32_t& top,
        int32_t& width,
        int32_t& height
) {
    auto width_scale = static_cast<float>(surface_width) / static_cast<float>(frame_width);
    auto height_scale = static_cast<float>(surface_height) / static_cast<float>(frame_height);
    auto scale = content_mode == bnb::player_api::interfaces::output::content_mode::aspect_fill
            ? std::fmax(width_scale, height_scale)
            : std::fmin(width_scale, height_scale);
    width = static_cast<int32_t>(static_cast<float>(frame_width) * scale);
    height = static_cast<int32_t>(static_cast<float>(frame_height) * scale);
    left = (surface_width - width) / 2;
    top = ((surface_height - height) / 2);
}

}

namespace bnb::player_api
{

    /* window_output::window_output */
    window_output::window_output()
    {
    }
    
    /* window_output::~window_output */
    window_output::~window_output()
    {
    }

    /* window_output::set_content_mode */
    void window_output::set_content_mode(content_mode new_content_mode)
    {
        m_content_mode = new_content_mode;
    }

    /* window_output::set_orientation */
    void window_output::set_orientation(orientation orient, bool mirroring)
    {
        m_orientation = orient;
        m_mirroring = mirroring;
    }

    /* window_output::present */
    void window_output::present(const render_target_sptr render_target)
    {
        int32_t x, y, w, h;
        calculate_frame_layout(m_window_width, m_window_height, render_target->get_render_width(), render_target->get_render_height(), m_content_mode, x, y, w, h);
        render_target->present(x, y, w, h, m_orientation, m_mirroring);
    }

    /* window_output::set_window_size */
    void window_output::set_window_size(int32_t width, int32_t height)
    {
        m_window_width = width;
        m_window_height = height;
    }

} /* namespace bnb::player_api */
