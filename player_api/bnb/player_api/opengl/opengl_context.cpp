#include <bnb/player_api/opengl/opengl_context.hpp>

#include <bnb/effect_player/utility.hpp>

namespace bnb::player_api
{

    /* opengl_context::opengl_context */
    opengl_context::opengl_context()
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
        
        glfwMakeContextCurrent(m_context);
        if (0 == gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("gladLoadGLLoader error");
        }
        
        bnb::utility::load_gl_functions();
    }

    /* opengl_context::~opengl_context */
    opengl_context::~opengl_context()
    {
        glfwDestroyWindow(m_context);
        m_context = nullptr;
    }

    /* opengl_context::activate */
    void opengl_context::activate()
    {
        glfwMakeContextCurrent(m_context);
    }

    /* opengl_context::deactivate */
    void opengl_context::deactivate()
    {
        glfwMakeContextCurrent(nullptr);
    }
    
    void opengl_context::swap_buffers()
    {
        glfwSwapBuffers(m_context);
    }
    
    void* opengl_context::get_sharing_context()
    {
        return reinterpret_cast<void*>(m_context);
    }

} /* namespace bnb::player_api */
