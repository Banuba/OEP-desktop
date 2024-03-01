#pragma once

#include <bnb/player_api/interfaces/orientation.hpp>

namespace bnb::player_api
{
    
    class opengl_frame_surface_handler
    {
    public:
        opengl_frame_surface_handler();
        ~opengl_frame_surface_handler();

        void set_orientation(orientation orient, bool mirroring);

        void draw_surface();

    private:
        uint32_t m_vbo {0};
        uint32_t m_vao {0};
        int32_t m_drawing_surface_geometry_offset {0};
    }; /* class opengl_frame_surface_handler */

} /* namespace bnb::player_api */
