#pragma once

#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/player_api/output/output_base.hpp>

namespace bnb::player_api
{

    class window_output : public bnb::player_api::interfaces::output, public output_base
    {
    public:
        window_output() = default;

        ~window_output() = default;

        void present(const render_target_sptr& render_target) override;

        void set_frame_layout(int32_t left, int32_t top, int32_t width, int32_t height);

    protected:
        int32_t m_frame_left {0};
        int32_t m_frame_top {0};
        int32_t m_frame_width {0};
        int32_t m_frame_height {0};
    }; /* class window_output */

} /* namespace bnb::player_api */

