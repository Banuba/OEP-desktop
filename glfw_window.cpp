#include "glfw_window.hpp"

#include <glad/glad.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

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

    using namespace bnb::example;

    glfw_event construct_event(glfw_event_t t, GLFWwindow* w)
    {
        glfw_event e;
        e.type = t;
        e.window = w;
        e.pos_x = e.pos_y = e.keyboard_mods = 0;
        return e;
    }

    glfw_event construct_event(glfw_event_t t, GLFWwindow* w, double a, double b)
    {
        glfw_event e;
        e.type = t;
        e.window = w;
        e.scroll_x = a;
        e.scroll_y = b;
        e.keyboard_mods = 0;
        return e;
    }

    glfw_event construct_event(glfw_event_t t, GLFWwindow* w, const char** a, int32_t b)
    {
        glfw_event e;
        e.type = t;
        e.window = w;
        e.file_paths = a;
        e.file_count = b;
        e.keyboard_mods = 0;
        return e;
    }

    glfw_event construct_event(glfw_event_t t, GLFWwindow* w, int32_t a, int32_t b, int32_t c = 0)
    {
        glfw_event e;
        e.type = t;
        e.window = w;
        e.pos_x = a;
        e.pos_y = b;
        e.keyboard_mods = c;
        return e;
    }

    glfw_event construct_event(glfw_event_t t, GLFWwindow* w, uint32_t a)
    {
        glfw_event e;
        e.type = t;
        e.window = w;
        e.codepoint = a;
        e.pos_y = e.keyboard_mods = 0;
        return e;
    }

    void send_event(const glfw_event& e)
    {
        auto* cb_raw_ptr = glfwGetWindowUserPointer(e.window);
        if (cb_raw_ptr) {
             (*reinterpret_cast<glfw_window::glfw_event_callback*>(cb_raw_ptr))(e);
        }
    }

} /* namespace */

namespace bnb::example
{

    glfw_window::glfw_window(const std::string_view& title)
    {
        // Init glfw for glfw_window
        if (!glfwInit()) {
            throw std::runtime_error("glfwInit() error");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_DEPTH_BITS, 24);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

        if ((m_window = glfwCreateWindow(1280, 720, title.data(), nullptr, nullptr)) == nullptr) {
            throw std::runtime_error("glfwCreateWindow() error");
        }

        glfwMakeContextCurrent(m_window);

        glfwSetErrorCallback(glfw_error_callback);
        glfwSwapInterval(0);

        if (0 == gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("gladLoadGLLoader() error");
        }

        glfwMakeContextCurrent(nullptr);
    }

    glfw_window::~glfw_window()
    {
        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    /* glfw_window::make_context_current */
    void glfw_window::make_context_current()
    {
        glfwMakeContextCurrent(m_window);
    }

    /* glfw_window::make_nothing_current */
    void glfw_window::make_nothing_current()
    {
        glfwMakeContextCurrent(nullptr);
    }

    /* glfw_window::swap_buffers */
    void glfw_window::swap_buffers()
    {
        glfwSwapBuffers(m_window);
    }

    /* glfw_window::set_glfw_events_callback */
    void glfw_window::set_glfw_events_callback(glfw_event_callback callback)
    {
        m_glfw_event_callback = callback;
        if (callback == nullptr) {
            untrack_events();
        } else {
            track_events();
        }
    }

    /* glfw_window::show_window_and_run_events_loop */
    void glfw_window::show_window_and_run_events_loop()
    {
        int32_t x, y, w, h;
        calculate_window_size_and_pos(x, y, w, h);
        glfwSetWindowPos(m_window, x, y);
        glfwSetWindowSize(m_window, w, h);
        glfwShowWindow(m_window);

        while (!glfwWindowShouldClose(m_window)) {
            glfwWaitEvents();
        }

        glfwTerminate();
    }

    /* glfw_window::track_events */
    void glfw_window::track_events()
    {
        glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(&m_glfw_event_callback));

        using t = bnb::example::glfw_event_t;
        glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int x, int y) {
            send_event(construct_event(t::window_move, window, x, y));
        });
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            send_event(construct_event(t::window_resize, window, width, height));
        });
        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
            send_event(construct_event(t::window_close, window));
        });
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            send_event(construct_event(t::framebuffer_resize, window, width, height));
        });
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
            assert(action == GLFW_PRESS || action == GLFW_RELEASE);
            send_event(construct_event(action == GLFW_PRESS ? t::mouse_press : t::mouse_release, window, button, mods));
        });
        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
            send_event(construct_event(t::mouse_move, window, static_cast<int32_t>(x), static_cast<int32_t>(y)));
        });
        glfwSetCursorEnterCallback(m_window, [](GLFWwindow* window, int entered) {
            send_event(construct_event(entered ? t::mouse_hower : t::mouse_left, window));
        });
        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y) {
            send_event(construct_event(t::scroll, window, x, y));
        });
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            assert(action == GLFW_PRESS || action == GLFW_RELEASE || action == GLFW_REPEAT);
            auto type = action == GLFW_PRESS ? t::key_press : (action == GLFW_RELEASE ? t::key_release : t::key_repeat);
            send_event(construct_event(type, window, key, scancode, mods));
        });
        glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
            send_event(construct_event(t::input_codepoint, window, codepoint));
        });
        glfwSetDropCallback(m_window, [](GLFWwindow* window, int count, const char** paths) {
            send_event(construct_event(t::drop_files, window, paths, count));
        });
    }

    /* glfw_window::untrack_events */
    void glfw_window::untrack_events()
    {
        glfwSetWindowUserPointer(m_window, nullptr);
        glfwSetWindowPosCallback(m_window, nullptr);
        glfwSetWindowSizeCallback(m_window, nullptr);
        glfwSetWindowCloseCallback(m_window, nullptr);
        glfwSetFramebufferSizeCallback(m_window, nullptr);
        glfwSetMouseButtonCallback(m_window, nullptr);
        glfwSetCursorPosCallback(m_window, nullptr);
        glfwSetCursorEnterCallback(m_window, nullptr);
        glfwSetScrollCallback(m_window, nullptr);
        glfwSetKeyCallback(m_window, nullptr);
        glfwSetCharCallback(m_window, nullptr);
        glfwSetDropCallback(m_window, nullptr);
    }

} /* namespace bnb::example */
