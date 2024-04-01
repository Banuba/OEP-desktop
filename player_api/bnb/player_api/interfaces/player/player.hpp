#pragma once

#include <bnb/player_api/interfaces/player.hpp>
#include <bnb/player_api/interfaces/render_target.hpp>

namespace bnb::player_api
{

    class BNB_EXPORT player
        : public interfaces::player
    {
    public:
        static std::shared_ptr<player> create(const render_target_sptr& render_target, const rendering_process_sptr& rendering_process);
    }; // class player

} // namespace bnb::player_api
