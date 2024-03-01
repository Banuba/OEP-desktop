#pragma once

#include <bnb/player_api/interfaces/output.hpp>

namespace bnb::player_api
{

    class window_output : public bnb::player_api::interfaces::output
    {
    public:
        window_output();
        
        ~window_output();

        void set_content_mode(content_mode new_content_mode) override;

        void set_orientation(orientation orient, bool mirroring) override;

        void present(const render_target_sptr render_target) override;
    };

} /* namespace bnb::player_api */

