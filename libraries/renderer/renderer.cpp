#include "renderer.hpp"

using namespace bnb::render;

/* renderer::~renderer */
renderer::~renderer()
{
    stop_auto_rendering();
}

/* renderer::surface_changed */
void renderer::surface_changed(int32_t width, int32_t height)
{
    m_width = width;
    m_height = height;
    m_surface_changed = true;
}

/* renderer::update_texture */
void renderer::update_texture(GLuint texture)
{
    m_texture_id = texture;
    m_texture_updated = true;
}

/* renderer::start_auto_rendering */
void renderer::start_auto_rendering(GLFWwindow* window)
{
    auto thread_func = [this, window]() {
        using namespace std::chrono_literals;
        if (m_auto_rendering_is_running) {
            throw std::runtime_error("auto rendering already is runing");
        }
        m_auto_rendering_is_running = true;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        initialize();

        while (m_auto_rendering_is_running) {
            if (m_surface_changed) {
                glViewport(0, 0, m_width, m_height);
                m_surface_changed = false;
            }
            if (m_texture_updated) {
                draw_texture(m_texture_id);
                m_texture_updated = false;
                glfwSwapBuffers(window);
            } else {
                std::this_thread::sleep_for(1us);
            }
        }

        shutdown();
        glfwMakeContextCurrent(nullptr);
    };

    m_auto_rendering_thread = std::thread(thread_func);
}

/* renderer::stop_auto_rendering */
void renderer::stop_auto_rendering()
{
    if (m_auto_rendering_is_running) {
        m_auto_rendering_is_running = false;
        m_auto_rendering_thread.join();
    }
}

/* renderer::initialize */
void renderer::initialize()
{
    // clang-format off
    static const char* vertex_shader_program =
        "precision highp float;\n "
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 vTexCoord;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos, 1.0);\n"
        "  vTexCoord = aTexCoord;\n"
        "}\n";

    static const char* fragment_shader_program =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "  FragColor = texture(uTexture, vTexCoord);\n"
        "}\n";

    static const float drawing_plane_coords[] = {
        /* verical flip 0 rotation 0deg */
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  /* top right */
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  /* bottom right */
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, /* top left */
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, /* bottom left */
    };
    // clang-format on

    m_program = std::make_unique<bnb::oep::program>("", vertex_shader_program, fragment_shader_program);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(drawing_plane_coords), drawing_plane_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* renderer::shutdown */
void renderer::shutdown()
{
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    m_program = nullptr;
}

/* renderer::draw_texture */
void renderer::draw_texture(GLuint texture)
{
    m_program->use();

    glActiveTexture(GLenum(GL_TEXTURE0));
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    m_program->unuse();
}
