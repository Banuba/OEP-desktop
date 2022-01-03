#include "render_context.hpp"

#include <bnb/effect_player/utility.hpp>

namespace bnb::oep
{

    /* interfaces::render_context::create */
    render_context_sptr bnb::oep::interfaces::render_context::create()
    {
        return std::make_shared<bnb::oep::render_context>();
    }

    /* render_context::render_context */
    render_context::render_context()
    {
        // Init glfw for glfw_window
        if (!glfwInit()) {
            throw std::runtime_error("glfwInit() error");
        }

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        m_context = glfwCreateWindow(1, 1, "", nullptr, nullptr);
        if (!m_context) {
            throw std::runtime_error("glfwCreateWindow() error");
        }
    }

    /* render_context::~render_context */
    render_context::~render_context()
    {
        glfwDestroyWindow(m_context);
    }

    /* render_context::create_context */
    void render_context::create_context()
    {
        glfwMakeContextCurrent(m_context);
        if (0 == gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("gladLoadGLLoader error");
        }
        bnb::utility::load_glad_functions((GLADloadproc) glfwGetProcAddress);
    }

    /* render_context::activate */
    void render_context::activate()
    {
        if (m_context) {
            glfwMakeContextCurrent(m_context);
        }
    }

    /* render_context::deactivate */
    void render_context::deactivate()
    {
        glfwMakeContextCurrent(nullptr);
    }

    /* render_context::delete_context */
    void render_context::delete_context()
    {}

    /* render_context::get_sharing_context */
    void * render_context::get_sharing_context()
    {
        return m_context;
    }

} /* namespace bnb::oep */
