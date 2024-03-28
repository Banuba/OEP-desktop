#pragma once

#include <bnb/player_api/types/orientation.hpp>

namespace bnb::player_api
{

    class orientable
    {
    public:
        virtual void set_orientation(orientation orient, bool mirroring = false) = 0;
    }; // class orientable

} // namespace bnb::player_api
