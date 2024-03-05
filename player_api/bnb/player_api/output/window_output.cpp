#include <bnb/player_api/output/window_output.hpp>

#include <bnb/player_api/opengl/opengl.hpp>
#include <bnb/player_api/utils/utils.hpp>

namespace bnb::player_api
{

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

    /* window_output::set_window_size */
    void window_output::set_window_size(int32_t width, int32_t height)
    {
        m_window_width = width;
        m_window_height = height;
    }

    /* window_output::present */
    void window_output::present(const render_target_sptr& render_target)
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        int32_t x, y, w, h;
        auto swap_sizes = m_orientation == orientation::left || m_orientation == orientation::right;
        auto frame_width = swap_sizes ? render_target->get_render_height() : render_target->get_render_width();
        auto frame_height = swap_sizes ? render_target->get_render_width() : render_target->get_render_height();
        calculate_frame_layout(m_window_width, m_window_height, frame_width, frame_height, m_content_mode, x, y, w, h);
        auto* mat = get_static_rotation_mat4(m_orientation, m_mirroring);
        render_target->present(x, y, w, h, mat);

        render_target->swap_buffers();
    }

} /* namespace bnb::player_api */
