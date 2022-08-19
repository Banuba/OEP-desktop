#include "frame_surface_handler.hpp"

namespace bnb::render
{

    const float bnb::render::frame_surface_handler::vertices[2][frame_surface_handler::v_size][5 * 4] =
    {{ /* verical flip 0 */
    {
            // positions        // texture coords
             1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // top right
             1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
            -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // top left
    },
    {
            // positions        // texture coords
             1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // top right
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // bottom left
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
    },
    {
            // positions        // texture coords
             1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top left
    },
    {
            // positions        // texture coords
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // top left
    }
    },
    { /* verical flip 1 */
    {
            // positions        // texture coords
             1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top right
             1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // bottom left
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top left
    },
    {
            // positions        // texture coords
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // top right
             1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // bottom right
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // bottom left
            -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top left
    },
    {
            // positions        // texture coords
             1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top right
             1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // bottom right
            -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // bottom left
            -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // top left
    },
    {
            // positions        // texture coords
             1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top right
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // bottom right
            -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom left
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top left
    }
    }};

    /* frame_surface_handler::frame_surface_handler */
    frame_surface_handler::frame_surface_handler(camera_orientation orientation, bool is_y_flip)
        : m_orientation(static_cast<uint32_t>(orientation))
        , m_y_flip(static_cast<uint32_t>(is_y_flip))
    {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[m_y_flip][m_orientation]), vertices[m_y_flip][m_orientation], GL_STATIC_DRAW);

        // clang-format off

        unsigned int indices[] = {
            // clang-format off
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
            // clang-format on
        };

        // clang-format on

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
        
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    /* frame_surface_handler::~frame_surface_handler */
    frame_surface_handler::~frame_surface_handler()
    {
        if (m_vao != 0)
            glDeleteVertexArrays(1, &m_vao);

        if (m_vbo != 0)
            glDeleteBuffers(1, &m_vbo);

        if (m_ebo != 0)
            glDeleteBuffers(1, &m_ebo);

        m_vao = 0;
        m_vbo = 0;
        m_ebo = 0;
    }

    /* frame_surface_handler::draw() */
    void frame_surface_handler::draw()
    {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

}/* namespace bnb::render */

