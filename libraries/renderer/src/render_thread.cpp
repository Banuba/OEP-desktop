#include "render_thread.hpp"

#include <libyuv.h>

namespace render {

render_thread::render_thread(GLFWwindow* window, int32_t width, int32_t height)
    : m_window(window)
    , m_thread([this]() { thread_func(); })
    , m_cancellation_flag(false)
    , m_cur_width(width)
    , m_cur_height(height) {}

render_thread::~render_thread()
{
    m_cancellation_flag = true;
    m_thread.join();
}

void render_thread::surface_changed(int32_t width, int32_t height)
{
    m_cur_width = width;
    m_cur_height = height;

    if (m_renderer) {
        m_renderer->surface_changed(width, height);
    }
}

void render_thread::update_data(bnb::full_image_t image)
{
    const auto& yuv = image.get_data<bnb::yuv_image_t>();
    m_cur_y_plane = yuv.y_plane;
    m_cur_uv_plane = yuv.uv_plane;
}

void render_thread::update_data(bnb::data_t data)
{
    libyuv::ABGRToNV12(data.data.get(),
               m_cur_width * 4,
               m_cur_y_plane.get(),
               m_cur_width,
               m_cur_uv_plane.get(),
               m_cur_width,
               m_cur_width,
               m_cur_height);
}

void render_thread::update_context()
{
    if (m_renderer) {
        m_renderer->update_camera_texture(m_cur_y_plane, m_cur_uv_plane);
    }
}

void render_thread::thread_func()
{
    using namespace std::chrono_literals;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    m_renderer = std::make_unique<bnb::renderer>(m_cur_width, m_cur_height);

    while (!m_cancellation_flag) {
        m_scheduler.run_all_tasks();
        const bool need_swap{ m_renderer->draw() != -1 };

        if (need_swap) {
            glfwSwapBuffers(m_window);
        } else {
            std::this_thread::sleep_for(1us);
        }
    }

    glfwMakeContextCurrent(nullptr);
}

} //render