#include <bnb/player_api/output/window_output.hpp>

namespace bnb::player_api
{

    /* window_output::set_window_size */
    void window_output::set_frame_layout(int32_t left, int32_t top, int32_t width, int32_t height)
    {
        m_frame_left = left;
        m_frame_top = top;
        m_frame_width = width;
        m_frame_height = height;
    }

    /* window_output::present */
    void window_output::present(const render_target_sptr& render_target)
    {
        int32_t x, y, w, h;
        oriented_frame_layout(render_target, m_frame_width, m_frame_height, x, y, w, h);
        render_target->present_to_screen(x + m_frame_left, y + m_frame_top, w, h, get_orientation_matrix());
    }

} /* namespace bnb::player_api */
