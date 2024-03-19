#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include "CoreFoundation/CoreFoundation.h"
#endif

#include <bnb/effect_player/utility.hpp>

#include <bnb/player_api/opengl/opengl.hpp>
#include <bnb/player_api/player/player.hpp>
#include <bnb/player_api/input/stream_input.hpp>
#include <bnb/player_api/input/live_input.hpp>
#include <bnb/player_api/input/photo_input.hpp>
#include <bnb/player_api/output/window_output.hpp>
#include <bnb/player_api/output/texture_output.hpp>
#include <bnb/player_api/output/opengl_frame_output.hpp>
#include <bnb/player_api/render_target/opengl_render_target.hpp>
#include <bnb/player_api/types/pixel_buffer_format.hpp>

#include <bnb/spal/camera/base.hpp>
#include "glfw_window.hpp"
#include "glfw_context.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third/stb/stb_image_write.h"

#include "graphical_user_interface.hpp"
#include <libyuv.h>

#include <thread>

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
            case t::nv12_bt601_full:
            case t::nv12_bt601_video:
            case t::nv12_bt709_full:
            case t::nv12_bt709_video:
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
            case t::i420_bt601_full:
            case t::i420_bt601_video:
            case t::i420_bt709_full:
            case t::i420_bt709_video:
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
    auto thread_func = [f]() {
        f();
    };
    auto thread = std::thread(thread_func);
    thread.detach();
}

int main()
{
    // The usage of this class is necessary in order to properly initialize and deinitialize Banuba SDK
    bnb::utility utility(get_resources_folders(), BNB_CLIENT_TOKEN);

    auto main_window = std::make_shared<bnb::example::glfw_window>("Player API Example");
    auto gui = std::make_shared<bnb::example::graphical_user_interface>(main_window);

    auto context = std::make_shared<bnb::example::glfw_context>(main_window);
    auto render_target = std::make_shared<bnb::player_api::opengl_render_target>(context);
    auto player = std::make_shared<bnb::player_api::player>(render_target);
    auto input = std::make_shared<bnb::player_api::live_input>();
    auto window_output = std::make_shared<bnb::player_api::window_output>();

    auto frame_output = std::make_shared<bnb::player_api::opengl_frame_output>([player](const bnb::player_api::output_sptr& self, const bnb::player_api::pixel_buffer_sptr& pb) {
        std::string file_path = std::string("/Users/petrkulbaka/work/cpp_player_api/build/") + bnb::player_api::pixel_buffer_format_to_str(pb->get_format());
        run_async([file_path, pb]() {
            save_pixel_buffer_to_file(file_path, pb);
        });
        self->deactive();
    }, bnb::player_api::pixel_buffer_format::nv12_bt709_full);
    frame_output->set_orientation(bnb::player_api::orientation::up, false);
    frame_output->deactive();

    player->use(input, window_output);
    player->add_output(frame_output);
    player->load_async("effects/DebugFRX");
//    player->set_render_mode(bnb::player_api::interfaces::player::render_mode::manual);

    player->set_render_status_callback([gui, main_window](int64_t frame_number) {
        if (frame_number >= 0) {
            gui->draw();
            main_window->swap_buffers();
            GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
        }
    });

    auto camera = bnb::create_camera_device([input](bnb::full_image_t image) {
        auto pb = bnb::example::full_image_to_pixel_buffer(image);
        input->push(pb);
    }, 0);

    gui->add_output_control(window_output, "Screen output");

    main_window->set_glfw_events_callback([window_output, gui, frame_output, player](const bnb::example::glfw_event& e) {
        if (e.type == bnb::example::glfw_event_t::framebuffer_resize) {
            window_output->set_frame_layout(0, 0, e.size_width, e.size_height);
        } else if (e.type == bnb::example::glfw_event_t::key_press && e.keyboard_key == GLFW_KEY_F1) {
            gui->switch_show_hide_gui();
        } else if (e.type == bnb::example::glfw_event_t::key_press && e.keyboard_key == GLFW_KEY_S) {
            frame_output->active();
        } else if (e.type == bnb::example::glfw_event_t::key_press && e.keyboard_key == GLFW_KEY_D) {
            //player->render();
        }
        gui->on_glfw_event(e);
    }); 

    main_window->show_window_and_run_events_loop();

    return 0;
}
