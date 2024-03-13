#pragma once

#include <bnb/player_api/opengl/opengl.hpp>

#include <cstdint>

namespace bnb::player_api
{

    class opengl_renderbuffer
    {
    public:
        opengl_renderbuffer();
        ~opengl_renderbuffer();

        void prepare(int32_t width, int32_t height);

        int32_t get_width() const noexcept;
        int32_t get_height() const noexcept;

        uint32_t get_texture() const noexcept;

    private:
        uint32_t m_framebuffer {0};
        uint32_t m_texture {0};

        int32_t m_width {0};
        int32_t m_height {0};
    }; /* class opengl_renderbuffer */

} /* namespace bnb::player_api */
