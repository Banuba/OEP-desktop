#include <bnb/player_api/opengl/opengl_frame_surface_handler.hpp>

#include <bnb/player_api/opengl/opengl.hpp>

namespace
{
    
    constexpr int32_t drawing_plane_xyz_length = 3;
    constexpr int32_t drawing_plane_xyz_offset = 0;
    constexpr int32_t drawing_plane_uv_length = 2;
    constexpr int32_t drawing_plane_uv_offset = sizeof(float) * drawing_plane_xyz_length;
    constexpr int32_t drawing_plane_vert_count = 4;
    constexpr int32_t drawing_plane_count = 8;
    constexpr int32_t drawing_plane_coords_per_vert = drawing_plane_xyz_length + drawing_plane_uv_length;
    constexpr int32_t drawing_plane_vert_stride = sizeof(float) * drawing_plane_coords_per_vert;

    // clang-format off
    static const float drawing_plane_coords[] = {
        /* X      Y     Z     U     V */
        /* mirroring 0, 0 degrees */
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, /* vertex 0 bottom left */
        1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, /* vertex 1 bottom right */
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, /* vertex 2 top left */
        1.0f,   1.0f, 0.0f, 1.0f, 0.0f, /* vertex 3 top right */
        /* mirroring 0, 90 degrees */
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, /* vertex 0 bottom left */
        1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, /* vertex 1 bottom right */
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, /* vertex 2 top left */
        1.0f,   1.0f, 0.0f, 1.0f, 1.0f, /* vertex 3 top right */
        /* mirroring 0, 180 degrees */
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, /* vertex 0 bottom left */
        1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, /* vertex 1 bottom right */
        -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, /* vertex 2 top left */
        1.0f,   1.0f, 0.0f, 0.0f, 1.0f, /* vertex 3 top right */
        /* mirroring 0, 270 degrees */
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, /* vertex 0 bottom left */
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, /* vertex 1 bottom right */
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, /* vertex 2 top left */
        1.0f,   1.0f, 0.0f, 0.0f, 0.0f, /* vertex 3 top right */

        /* mirroring 1, 0 degrees */
        1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, /* vertex 0 bottom left */
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, /* vertex 1 bottom right */
        1.0f,   1.0f, 0.0f, 0.0f, 0.0f, /* vertex 2 top left */
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, /* vertex 3 top right */
        /* mirroring 1, 90 degrees */
        1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, /* vertex 0 bottom left */
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, /* vertex 1 bottom right */
        1.0f,   1.0f, 0.0f, 1.0f, 0.0f, /* vertex 2 top left */
        -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, /* vertex 3 top right */
        /* mirroring 1, 180 degrees */
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, /* vertex 0 bottom left */
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, /* vertex 1 bottom right */
        1.0f,   1.0f, 0.0f, 1.0f, 1.0f, /* vertex 2 top left */
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, /* vertex 3 top right */
        /* mirroring 1, 270 degrees */
        1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, /* vertex 0 bottom left */
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, /* vertex 1 bottom right */
        1.0f,   1.0f, 0.0f, 0.0f, 1.0f, /* vertex 2 top left */
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f  /* vertex 3 top right */
    };
    // clang-format on
}

namespace bnb::player_api
{
    
    /* opengl_frame_surface_handler::opengl_frame_surface_handler */
    opengl_frame_surface_handler::opengl_frame_surface_handler()
    {
        /* create and bind drawing geometry */
        GL_CALL(glGenVertexArrays(1, &m_vao));
        GL_CALL(glGenBuffers(1, &m_vbo));
        GL_CALL(glBindVertexArray(m_vao));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(drawing_plane_coords), drawing_plane_coords, GL_STATIC_DRAW));
        GL_CALL(glVertexAttribPointer(0, drawing_plane_xyz_length, GL_FLOAT, GL_FALSE, drawing_plane_vert_stride, reinterpret_cast<void*>(drawing_plane_xyz_offset)));
        GL_CALL(glVertexAttribPointer(1, drawing_plane_uv_length, GL_FLOAT, GL_FALSE, drawing_plane_vert_stride, reinterpret_cast<void*>(drawing_plane_uv_offset)));
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GL_CALL(glBindVertexArray(0));
    }

    /* opengl_frame_surface_handler::opengl_frame_surface_handler */
    opengl_frame_surface_handler::~opengl_frame_surface_handler()
    {
        if (glIsBuffer(m_vbo)) {
            GL_CALL(glDeleteBuffers(1, &m_vbo));
            m_vbo = 0;
        }
        if (glIsVertexArray(m_vao)) {
            GL_CALL(glDeleteVertexArrays(1, &m_vao));
            m_vao = 0;
        }
    }

    /* opengl_frame_surface_handler::draw_surface */
    void opengl_frame_surface_handler::draw_surface(orientation orient, bool mirroring)
    {
        int32_t mirroring_geometry_offset = mirroring ? 4 * drawing_plane_vert_count : 0;
        int32_t drawing_plane_geometry_offset = static_cast<int32_t>(orient) / 90 * drawing_plane_vert_count + mirroring_geometry_offset;
        GL_CALL(glBindVertexArray(m_vao));
        GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, drawing_plane_geometry_offset, drawing_plane_vert_count));
        GL_CALL(glBindVertexArray(0));
    }

} /* namespace bnb::player_api */
