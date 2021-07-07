#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <thread>

#include "renderer.hpp"

namespace bnb::render
{
    class render_thread
    {
    public:
        render_thread(GLFWwindow* window, int32_t width, int32_t height);

        ~render_thread();

        void surface_changed(int32_t width, int32_t height);
        void update_data(int texture_id);

    private:
        void thread_func(int32_t width, int32_t height);

        std::unique_ptr<renderer> m_renderer { nullptr };
        GLFWwindow* m_window;
        std::thread m_thread;
        std::atomic<bool> m_cancellation_flag;
    };
} // bnb::render

using render_t_sptr = std::shared_ptr<bnb::render::render_thread>;
using render_t_wptr = std::weak_ptr<bnb::render::render_thread>;