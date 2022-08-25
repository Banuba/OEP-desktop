#pragma once

#include <interfaces/render_context.hpp>
#include "libraries/utils/glfw_window.hpp"
#include "libraries/renderer/render_thread.hpp"

namespace bnb::oep
{

    class render_context : public bnb::oep::interfaces::render_context
    {
    public:
        render_context();
        ~render_context();

        void create_context() override;
        void activate() override;
        void deactivate() override;
        void delete_context() override;

        void * get_sharing_context() override;
    private:
        GLFWwindow * m_context;
    }; /* class render_context */

} /* namespace bnb::oep */
