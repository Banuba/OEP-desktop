#include <bnb/player_api/output/window_output.hpp>

namespace bnb::player_api
{

    /* window_output::set_window_size */
    void window_output::set_window_size(int32_t width, int32_t height)
    {
        m_window_width = width;
        m_window_height = height;
    }

    /* window_output::present */
    void window_output::present(const render_target_sptr& render_target)
    {
        int32_t x, y, w, h;
        oriented_frame_layout(render_target, m_window_width, m_window_height, x, y, w, h);
        render_target->present_to_screen(x, y, w, h, get_orientation_matrix());
    }

} /* namespace bnb::player_api */
