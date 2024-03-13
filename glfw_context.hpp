#pragma once

#include <bnb/player_api/interfaces/render_context.hpp>
#include "glfw_window.hpp"

namespace bnb::example
{

    class glfw_context : public bnb::player_api::interfaces::render_context
    {
    public:
        glfw_context(const std::shared_ptr<glfw_window>& window)
            : m_glfw_window(window)
        {
        }

        ~glfw_context() = default;

        void activate() override
        {
            m_glfw_window->make_context_current();
        }

        void deactivate() override
        {
            m_glfw_window->make_nothing_current();
        }

    private:
        std::shared_ptr<glfw_window> m_glfw_window;
    }; /* class glfw_context */

} /* namespace bnb::example */
