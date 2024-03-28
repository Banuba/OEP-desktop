#pragma once

#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/player_api/output/orientable.hpp>
#include <bnb/player_api/output/scalable.hpp>

namespace bnb::player_api
{

    class window_output
        : public bnb::player_api::interfaces::output
        , public bnb::player_api::orientable
        , public bnb::player_api::scalable
    {
    public:
        static std::shared_ptr<window_output> create();

        virtual void set_frame_layout(int32_t left, int32_t top, int32_t width, int32_t height) = 0;
    }; // class window_output

} // namespace bnb::player_api
