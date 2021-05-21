#include "glfw_window.hpp"

#include <bnb/effect_player/utility.hpp>
#include <bnb/utils/defs.hpp>
#include <bnb/postprocess/interfaces/postprocess_helper.hpp>

#include <glad/glad.h>

using namespace bnb;

glfw_window::glfw_window(const std::string& title, GLFWwindow* share)
{
    init();
    try {
        create_window(title, share);

        glfwMakeContextCurrent(m_window);

        load_glad_functions();

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glfwMakeContextCurrent(nullptr);
    } catch (...) {
        glfwTerminate();
        throw;
    }
}

glfw_window::~glfw_window()
{
    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void glfw_window::show(uint32_t width_hint, uint32_t height_hint)
{
    async::spawn(
        m_scheduler,
        [this, width_hint, height_hint]() {
            glfwSetWindowSize(m_window, width_hint, height_hint);
            glfwSetWindowPos(m_window, 0, 0);
            glfwShowWindow(m_window);
        });

    glfwPostEmptyEvent();
}

void glfw_window::run_main_loop()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwWaitEvents();
        m_scheduler.run_all_tasks();
    }
}

void glfw_window::init()
{
    if (GLFW_TRUE != glfwInit()) {
        throw std::runtime_error("glfwInit error");
    }
}

void glfw_window::create_window(const std::string& title, GLFWwindow* share)
{
    //
    // Choose OpenGL context
    //

    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    

#if BNB_OS_WINDOWS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#elif BNB_OS_MACOS || BNB_OS_LINUX
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    //
    // Create window
    //

    // The new window is hidden until is shown, so
    // initial width and height are just dummy values
    auto initial_window_width = 1;
    auto initial_window_height = 1;

    m_window = glfwCreateWindow(
        initial_window_width,
        initial_window_height,
        title.c_str(),
        nullptr,
        share);

    if (nullptr == m_window) {
        throw std::runtime_error("glfwCreateWindow error");
    }
}

void glfw_window::load_glad_functions()
{
#if BNB_OS_WINDOWS || BNB_OS_MACOS
    // it's only need for use while working with dynamic libs
    utility::load_glad_functions((GLADloadproc) glfwGetProcAddress);
    bnb::interfaces::postprocess_helper::load_glad_functions(reinterpret_cast<int64_t>(glfwGetProcAddress));
#endif

    if (0 == gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("gladLoadGLLoader error");
    }
}
