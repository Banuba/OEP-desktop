#include <bnb/player_api/output/texture_output.hpp>

namespace bnb::player_api
{

    /* texture_output::texture_output */
    texture_output::texture_output(const texture_ready_callback& callback)
        : m_callback(callback)
    {
    }

    /* texture_output::present */
    void texture_output::present(const output_sptr&, const render_target_sptr& render_target)
    {
        m_callback(render_target->get_output_texture());
    }

} // namespace bnb::player_api
