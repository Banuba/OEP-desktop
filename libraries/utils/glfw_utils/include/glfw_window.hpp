#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <async++.h>

class glfw_window
{
public:
    explicit glfw_window(const std::string& title);
    ~glfw_window();

    void show(uint32_t width_hint, uint32_t height_hint);
    void run_main_loop();

    [[nodiscard]] GLFWwindow* get_window() const
    {
        return m_window;
    }

private:
    // To execute scheduled tasks wake up main loop (glfwPostEmptyEvent)
    async::fifo_scheduler m_scheduler;
    GLFWwindow* m_window{};

    void init();
    void create_window(const std::string& title);
    void load_glad_functions();
};