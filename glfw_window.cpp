#include "glfw_window.hpp"

#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>

namespace
{

    void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error: %s\n", description);
    }

    void calculate_window_size_and_pos(int32_t& x, int32_t& y, int32_t& w, int32_t& h)
    {
        //
        // All calculation are based on GLFW's screen coordinates
        // so result behaviour is pretty similar on both retina and
        // non-retina screens
        //

        GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();

        if (primary_monitor == nullptr) {
            throw std::runtime_error("glfwGetPrimaryMonitor() error");
        }

        const GLFWvidmode* current_video_mode = glfwGetVideoMode(primary_monitor);

        if (current_video_mode == nullptr) {
            throw std::runtime_error("glfwGetVideoMode() error");
        }

        auto screen_height = static_cast<float>(current_video_mode->height);
        auto screen_width = static_cast<float>(current_video_mode->width);

        // Chose the window size so that it occupies the 3/4 of the screen's width.
        // Then shrink it if the height is too high. Aspect calculated by the hints is
        // always preserved.

        constexpr float aspect = 16.0f / 9.0f;

        float scaled_w = 0.8f * screen_width;
        float scaled_h = scaled_w / aspect;

        // put in the center of the screen
        x = static_cast<int32_t>(std::max((screen_width - scaled_w), 0.f) / 2.f);
        y = static_cast<int32_t>(std::max((screen_height - scaled_h), 0.f) / 2.f);
        w = static_cast<int32_t>(scaled_w);
        h = static_cast<int32_t>(scaled_h);
    }

} /* namespace */

namespace bnb
{

    glfw_window::glfw_window(const std::string_view& title)
    {
        // Init glfw for glfw_window
        if (!glfwInit()) {
            throw std::runtime_error("glfwInit() error");
        }

        glfwSetErrorCallback(glfw_error_callback);
        glfwSwapInterval(0);

        glfwWindowHint(GLFW_DEPTH_BITS, 0);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

        if ((m_window = glfwCreateWindow(1280, 720, title.data(), nullptr, nullptr)) == nullptr) {
            throw std::runtime_error("glfwCreateWindow() error");
        }

        glfwMakeContextCurrent(m_window);

        if (0 == gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("gladLoadGLLoader() error");
        }

        glfwMakeContextCurrent(nullptr);

        glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(&m_user_data));
    }

    glfw_window::~glfw_window()
    {
        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void glfw_window::show_and_run_events_loop()
    {
        int32_t x, y, w, h;
        calculate_window_size_and_pos(x, y, w, h);
        glfwSetWindowPos(m_window, x, y);
        glfwSetWindowSize(m_window, w, h);
        glfwShowWindow(m_window);

        auto glfw_resize_callback = [](GLFWwindow* window, int width, int height) {
            auto user_data_rawptr = reinterpret_cast<glfw_user_data_t*>(glfwGetWindowUserPointer(window));
            auto cb = user_data_rawptr->surface_resized_callback;
            if (cb != nullptr) {
                int32_t fb_width, fb_height;
                glfwGetFramebufferSize(window, &fb_width, &fb_height);
                cb(fb_width, fb_height);
            }
        };

        glfw_resize_callback(m_window, w, h);

        glfwSetWindowSizeCallback(m_window, glfw_resize_callback);

        while (!glfwWindowShouldClose(m_window)) {
            glfwWaitEvents();
        }
    }

} /* namespace bnb */
