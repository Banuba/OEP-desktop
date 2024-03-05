#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string_view>
#include <thread>

namespace bnb
{

    class glfw_window
    {
    public:
        using surface_resized_callback = std::function<void(int32_t width, int32_t height)>;
        
    public:
        struct glfw_user_data_t
        {
            surface_resized_callback surface_resized_callback;
        };
    public:
        glfw_window(const std::string_view& title);
        ~glfw_window();

        void set_surface_changed_callback(surface_resized_callback callback)
        {
            m_user_data.surface_resized_callback = callback;
        }

        void show_and_run_events_loop();

        [[nodiscard]] GLFWwindow* get_window() const
        {
            return m_window;
        }

    private:
        GLFWwindow* m_window {nullptr};
        glfw_user_data_t m_user_data;
    };

} /* namespace bnb */
