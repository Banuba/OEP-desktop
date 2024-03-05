#pragma once

#include <bnb/player_api/interfaces/output.hpp>

namespace bnb::player_api
{

    class window_output : public bnb::player_api::interfaces::output
    {
    public:
        window_output() = default;

        ~window_output() = default;

        void set_content_mode(content_mode new_content_mode) override;

        void set_orientation(orientation orient, bool mirroring = false) override;

        void present(const render_target_sptr& render_target) override;

        void set_window_size(int32_t width, int32_t height);

    private:
        content_mode m_content_mode {content_mode::aspect_fit};
        int32_t m_window_width {0};
        int32_t m_window_height {0};
        orientation m_orientation {orientation::up};
        bool m_mirroring {false};
    }; /* class window_output */

} /* namespace bnb::player_api */
