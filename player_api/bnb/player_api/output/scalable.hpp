#pragma once

#include <bnb/player_api/types/content_mode.hpp>

namespace bnb::player_api
{

    class scalable
    {
    public:
        virtual void set_content_mode(content_mode new_content_mode) = 0;
    }; // class scalable

} // namespace bnb::player_api
