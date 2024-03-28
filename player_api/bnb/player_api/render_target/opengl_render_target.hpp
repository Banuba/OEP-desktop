#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/player_api/interfaces/render_context.hpp>

namespace bnb::player_api
{

    class opengl_render_target
        : public bnb::player_api::interfaces::render_target
    {
    public:
        static std::shared_ptr<opengl_render_target> create(const render_context_sptr& context);
    }; // class opengl_render_target

} // namespace bnb::player_api
