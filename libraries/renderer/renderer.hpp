#pragma once

#include <memory>
#include <thread>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <opengl/program.hpp>

namespace bnb::render
{
    class renderer;
} /* namespace bnb::render */

using renderer_sptr = std::shared_ptr<bnb::render::renderer>;
using renderer_wptr = std::weak_ptr<bnb::render::renderer>;

namespace bnb::render
{
    class renderer
    {
    public:
        renderer() = default;
        
        ~renderer();

        void surface_changed(int32_t width, int32_t height);

        void update_texture(GLuint texture);

        void start_auto_rendering(GLFWwindow* window);

        void stop_auto_rendering();

    private:
        void initialize();

        void shutdown();

        void draw_texture(GLuint texture);

    private:
        std::thread m_auto_rendering_thread;

        std::unique_ptr<bnb::oep::program> m_program {nullptr};

        int32_t m_width {0};
        int32_t m_height {0};
        GLuint m_texture_id {0};
        GLuint m_vao {0};
        GLuint m_vbo {0};

        std::atomic_bool m_auto_rendering_is_running {false};
        std::atomic_bool m_texture_updated {false};
        std::atomic_bool m_surface_changed {false};
    };
} // namespace bnb::render
