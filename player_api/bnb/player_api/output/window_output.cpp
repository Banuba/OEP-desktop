#include <bnb/player_api/output/window_output.hpp>

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
    }

    void window_output::set_orientation(orientation orient, bool mirroring)
    {
    }

    void window_output::present(const render_target_sptr render_target)
    {
        render_target->present(0, 0, render_target->get_render_width(), render_target->get_render_height());
    }

} /* namespace bnb::player_api */
