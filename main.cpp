#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include "CoreFoundation/CoreFoundation.h"
#endif

#include <bnb/effect_player/utility.hpp>

#include <glad/glad.h>
#include <bnb/player_api/interfaces/player/player.hpp>
#include <bnb/player_api/interfaces/input/stream_input.hpp>
#include <bnb/player_api/interfaces/input/live_input.hpp>
#include <bnb/player_api/interfaces/input/photo_input.hpp>
#include <bnb/player_api/interfaces/output/window_output.hpp>
#include <bnb/player_api/interfaces/output/texture_output.hpp>
#include <bnb/player_api/interfaces/output/opengl_frame_output.hpp>
#include <bnb/player_api/interfaces/render_target/opengl_render_target.hpp>
#include <bnb/player_api/interfaces/types/pixel_buffer_format.hpp>

#include <bnb/spal/camera/base.hpp>
#include "glfw_window.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third/stb/stb_image_write.h"

#include "graphical_user_interface.hpp"
#include <libyuv.h>

#include <thread>
#include <type_traits>

#include "camera_utils.hpp"

#define BNB_CLIENT_TOKEN <#TOKEN#>

std::vector<std::string> get_resources_folders()
{
    std::vector<std::string> dirs;
#if defined(__APPLE__)
    // The BNB SDK framework includes BNB resources if not compiled with option to separate resources.
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef bundleURL = CFBundleCopyBundleURL(bundle);
    char path[PATH_MAX];
    Boolean success = CFURLGetFileSystemRepresentation(bundleURL, TRUE, (UInt8 *)path, PATH_MAX);
    assert(success);
    CFRelease(bundleURL);

    dirs.push_back(std::string(path) + "/Contents/Frameworks/BanubaEffectPlayer.framework/Resources/bnb-resources");
    dirs.push_back(std::string(path) + "/Contents/Resources");
#else
    dirs.push_back(BNB_RESOURCES_FOLDER);
#endif
    return dirs;
}

void save_pixel_buffer_to_file(std::string path, const bnb::player_api::pixel_buffer_sptr& pb)
{
    using t = bnb::player_api::pixel_buffer_format;
    switch (pb->get_format()) {
            case t::bpc8_rgb:
            case t::bpc8_bgr:
            case t::bpc8_rgba:
            case t::bpc8_bgra:
            case t::bpc8_argb:
                stbi_write_png(
                    (path + ".png").c_str(),
                    pb->get_width(),
                    pb->get_height(),
                    pb->get_bytes_per_pixel(),
                    pb->get_base_ptr(),
                    pb->get_bytes_per_row()
                );
                break;
            case t::nv12:
                {
                    int rgb_stride = pb->get_width() * 3;
                    size_t size = rgb_stride * pb->get_height();
                    uint8_t* raw_ptr = new uint8_t[size];
                    libyuv::NV12ToRGB24(
                        pb->get_base_ptr_of_plane(0),
                        pb->get_bytes_per_row_of_plane(0),
                        pb->get_base_ptr_of_plane(1),
                        pb->get_bytes_per_row_of_plane(1),
                        raw_ptr,
                        rgb_stride,
                        pb->get_width(),
                        pb->get_height()
                    );
                    stbi_write_png((path + ".png").c_str(), pb->get_width(), pb->get_height(), 3, raw_ptr, rgb_stride);
                    delete [] raw_ptr;
                    stbi_write_png((path + ".raw.png").c_str(), pb->get_bytes_per_row(), pb->get_height() + pb->get_height_of_plane(1), 1, pb->get_base_ptr(), pb->get_bytes_per_row());
                }
                break;
            case t::i420:
                {
                    int rgb_stride = pb->get_width() * 3;
                    size_t size = rgb_stride * pb->get_height();
                    uint8_t* raw_ptr = new uint8_t[size];
                    libyuv::I420ToRGB24(
                        pb->get_base_ptr_of_plane(0),
                        pb->get_bytes_per_row_of_plane(0),
                        pb->get_base_ptr_of_plane(1),
                        pb->get_bytes_per_row_of_plane(1),
                        pb->get_base_ptr_of_plane(2),
                        pb->get_bytes_per_row_of_plane(2),
                        raw_ptr,
                        rgb_stride,
                        pb->get_width(),
                        pb->get_height()
                    );
                    stbi_write_png((path + ".png").c_str(), pb->get_width(), pb->get_height(), 3, raw_ptr, rgb_stride);
                    delete [] raw_ptr;
                    stbi_write_png((path + "raw.png").c_str(), pb->get_bytes_per_row(), pb->get_height() + pb->get_height_of_plane(1), 1, pb->get_base_ptr(), pb->get_bytes_per_row());
                }
                break;
        }
}

void run_async(std::function<void()> f)
{
    auto thread = std::thread(f);
    thread.detach();
}

class glfw_renderer
    : public bnb::player_api::interfaces::render_delegate
{
public:
    glfw_renderer()
    {
        m_window = std::make_shared<bnb::example::glfw_window>("Player API Example");
        m_gui = std::make_shared<bnb::example::graphical_user_interface>(m_window);
        m_window->make_context_current();
        started();
        finished(0);
        m_window->make_nothing_current();
    }

    ~glfw_renderer() = default;

    std::shared_ptr<bnb::example::glfw_window> get_window()
    {
        return m_window;
    }

    std::shared_ptr<bnb::example::graphical_user_interface> get_gui()
    {
        return m_gui;
    }

    void activate() override
    {
        m_window->make_context_current();
    }

    void started() override
    {
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void finished(int64_t frame_number) override
    {
        if (frame_number != -1) {
            m_gui->draw();
            m_window->swap_buffers();
        }
        m_window->make_nothing_current();
    }

private:
    std::shared_ptr<bnb::example::glfw_window> m_window;
    std::shared_ptr<bnb::example::graphical_user_interface> m_gui;
}; // render_process

int main()
{
    // The usage of this class is necessary in order to properly initialize and deinitialize Banuba SDK
    bnb::utility utility(get_resources_folders(), BNB_CLIENT_TOKEN);

    auto renderer = std::make_shared<glfw_renderer>();

    auto render_target = bnb::player_api::opengl_render_target::create();
    auto player = bnb::player_api::player::create(30, render_target, renderer);
    auto input = bnb::player_api::live_input::create();
    auto window_output = bnb::player_api::window_output::create();

    auto frame_output = bnb::player_api::opengl_frame_output::create([player](const bnb::player_api::pixel_buffer_sptr& pb) {
        std::string file_path = std::string("/Users/petrkulbaka/work/cpp_player_api/build/_/") + bnb::player_api::pixel_buffer_format_to_str(pb->get_format()).data();
        run_async([file_path, pb]() {
            save_pixel_buffer_to_file(file_path, pb);
        });
    }, bnb::player_api::pixel_buffer_format::i420, false);
    frame_output->set_yuv_format_params(bnb::color_std::bt709, bnb::color_range::full);

    player->use(input).use(window_output).use(frame_output);
    player->load_async("effects/DebugFRX");

    auto camera = bnb::create_camera_device([input](bnb::full_image_t image) {
        auto pb = bnb::example::full_image_to_pixel_buffer(image);
        input->push(pb);
    }, 0);

    renderer->get_gui()->add_output_control(window_output, "Screen output");

    renderer->get_window()->set_glfw_events_callback([renderer, window_output, frame_output](const bnb::example::glfw_event& e) {
        if (e.type == bnb::example::glfw_event_t::framebuffer_resize) {
            window_output->set_frame_layout(0, 0, e.size_width, e.size_height);
        } else if (e.type == bnb::example::glfw_event_t::key_press && e.keyboard_key == GLFW_KEY_F1) {
            renderer->get_gui()->switch_show_hide_gui();
        } else if (e.type == bnb::example::glfw_event_t::key_press && e.keyboard_key == GLFW_KEY_S) {
            frame_output->once();
        }
        renderer->get_gui()->on_glfw_event(e);
    });

    renderer->get_window()->show_window_and_run_events_loop();

    return 0;
}
