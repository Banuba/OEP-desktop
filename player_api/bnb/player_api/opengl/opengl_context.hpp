#pragma once

#include <bnb/player_api/opengl/opengl.hpp>

namespace bnb::player_api
{

    class opengl_context
    {
    public:
        opengl_context();
        ~opengl_context();

        void activate();

        void deactivate();
        
        void swap_buffers();
        
        void* get_sharing_context();

    private:
        GLFWwindow* m_context {nullptr};
    }; /* class opengl_context */

} /* namespace bnb::player_api */
