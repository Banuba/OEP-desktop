#pragma once

#include <bnb/types/full_image.hpp>

#include "program.hpp"
#include "renderer_gl_context.hpp"

namespace bnb::render
{
    struct nv12_planes
    {
        color_plane y_plane;
        color_plane uv_plane;
    };

    class renderer
    {
    public:
        renderer(int width, int height);

        void surface_change(int32_t width, int32_t height);

        void update_data(full_image_t image);
        bool draw();

    private:
        void update_camera_texture();

        renderer_gl_context m_gl_context;
        program m_program;

        int m_width;
        int m_height;

        nv12_planes m_update_buffer;
        nv12_planes m_show_buffer;

        std::atomic<bool> m_rendering = false;
        std::atomic<bool> m_texture_updated = false;

        std::atomic<bool> m_surface_changed = false;
    };
} // bnb::render
