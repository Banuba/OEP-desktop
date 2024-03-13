#pragma once

#include <bnb/player_api/types/orientation.hpp>

namespace bnb::player_api
{
    
    class opengl_frame_surface_handler
    {
    public:
        opengl_frame_surface_handler();
        ~opengl_frame_surface_handler();

        void draw_surface();

    private:
        uint32_t m_vbo {0};
        uint32_t m_vao {0};
    }; /* class opengl_frame_surface_handler */

} /* namespace bnb::player_api */
