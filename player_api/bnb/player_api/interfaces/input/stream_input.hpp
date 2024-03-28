#pragma once

#include <bnb/player_api/interfaces/input.hpp>
#include <bnb/player_api/interfaces/input/pixels_provider.hpp>

namespace bnb::player_api
{

    class BNB_EXPORT stream_input
        : public virtual bnb::player_api::interfaces::input
        , public virtual bnb::player_api::pixels_provider
    {
    public:
        static std::shared_ptr<stream_input> create();
    }; // class stream_input

} // namespace bnb::player_api