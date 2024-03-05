#pragma once

#include <bnb/player_api/interfaces/render_context.hpp>
#include "glfw_window.hpp"

namespace bnb
{

    class opengl_context : public bnb::player_api::interfaces::render_context
    {
    public:
        opengl_context(const std::shared_ptr<glfw_window>& window)
            : m_glfw_window(window)
        {
        }

        ~opengl_context() = default;

        void activate() override
        {
            glfwMakeContextCurrent(m_glfw_window->get_window());
        }

        void deactivate() override
        {
            glfwMakeContextCurrent(nullptr);
        }

        void swap_buffers() override
        {
            glfwSwapBuffers(m_glfw_window->get_window());
        }

    private:
        std::shared_ptr<glfw_window> m_glfw_window;
    }; /* class opengl_context */

} /* namespace bnb */
