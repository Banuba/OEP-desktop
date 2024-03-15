#include <bnb/player_api/output/window_output.hpp>

#include <cmath>

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
    void window_output::present(const output_sptr&, const render_target_sptr& render_target)
    {
        int32_t x, y, w, h;
        oriented_frame_layout(render_target, m_frame_width, m_frame_height, x, y, w, h);
        render_target->prepare_to_screen_render();
        render_target->present(x + m_frame_left, y + m_frame_top, w, h, get_orientation_matrix());
    }

} /* namespace bnb::player_api */
