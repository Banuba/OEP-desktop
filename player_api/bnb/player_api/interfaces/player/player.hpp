#pragma once

#include <bnb/player_api/interfaces/player.hpp>
#include <bnb/player_api/interfaces/render_delegate.hpp>
#include <bnb/player_api/interfaces/render_target.hpp>

namespace bnb::player_api
{

    class BNB_EXPORT player
        : public interfaces::player
    {
    public:
        static std::shared_ptr<player> create(uint32_t fps, const render_target_sptr& render_target, const render_delegate_sptr& render_delegate);
    }; // class player

} // namespace bnb::player_api
