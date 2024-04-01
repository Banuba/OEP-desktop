#pragma once

#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/player_api/interfaces/output/orientable.hpp>
#include <bnb/player_api/interfaces/output/scalable.hpp>

namespace bnb::player_api
{

    class BNB_EXPORT window_output
        : public interfaces::output
        , public orientable
        , public scalable
    {
    public:
        static std::shared_ptr<window_output> create();

        virtual void set_frame_layout(int32_t left, int32_t top, int32_t width, int32_t height) = 0;
    }; // class window_output

} // namespace bnb::player_api
