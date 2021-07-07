#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <async++.h>

class glfw_window
{
public:
    explicit glfw_window(const std::string& title, GLFWwindow* share = nullptr);
    ~glfw_window();

    void set_resize_callback(std::function<void(int32_t w, int32_t h, int32_t w_glfw_buffer, int32_t h_glfw_buffer)> surface_changed);

    void show(uint32_t width_hint, uint32_t height_hint);
    void run_main_loop();

    [[nodiscard]] GLFWwindow* get_window() const
    {
        return m_window;
    }

private:
    void init();
    void create_window(const std::string& title, GLFWwindow* share = nullptr);
    void load_glad_functions();

    // To execute scheduled tasks wake up main loop (glfwPostEmptyEvent)
    async::fifo_scheduler m_scheduler;
    GLFWwindow* m_window{};

    std::function<void(int32_t w, int32_t h, int32_t w_glfw_buffer, int32_t h_glfw_buffer)> surface_changed_callback;

    inline static int32_t window_width = 1;
    inline static int32_t window_height = 1;

    inline static bool resized = false; 
};
