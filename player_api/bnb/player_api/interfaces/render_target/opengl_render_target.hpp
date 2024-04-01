#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>

namespace bnb::player_api
{

    class BNB_EXPORT opengl_render_target
        : public bnb::player_api::interfaces::render_target
    {
    public:
        static std::shared_ptr<opengl_render_target> create();
    }; // class opengl_render_target

} // namespace bnb::player_api
