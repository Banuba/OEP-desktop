#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include "CoreFoundation/CoreFoundation.h"
#endif

#include <bnb/effect_player/utility.hpp>

#include <bnb/player_api/player/player.hpp>
#include <bnb/player_api/input/stream_input.hpp>
#include <bnb/player_api/output/window_output.hpp>
#include <bnb/player_api/output/texture_output.hpp>
#include <bnb/player_api/render_target/opengl_render_target.hpp>

#include <bnb/spal/camera/base.hpp>
#include "glfw_window.hpp"
#include "glfw_context.hpp"

#include "graphical_user_interface.hpp"

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

int main()
{
    // The usage of this class is necessary in order to properly initialize and deinitialize Banuba SDK
    bnb::utility utility(get_resources_folders(), BNB_CLIENT_TOKEN);

    auto main_window = std::make_shared<bnb::example::glfw_window>("Player API Example");
    auto gui = std::make_shared<bnb::example::graphical_user_interface>(main_window);

    auto context = std::make_shared<bnb::example::glfw_context>(main_window);
    auto render_target = std::make_shared<bnb::player_api::opengl_render_target>(context);
    auto player = std::make_shared<bnb::player_api::player>(render_target);
    auto input = std::make_shared<bnb::player_api::stream_input>();
    auto window_output = std::make_shared<bnb::player_api::window_output>();
    auto texture_output = std::make_shared<bnb::player_api::texture_output>([gui](bnb::player_api::texture_t texture) {
//        gui->track_output
    });

    player->use(input, window_output);
    player->add_output(texture_output);
    player->play();
    player->load_async("effects/DebugFRX");

    player->set_render_status_callback([gui, main_window](int64_t frame_number) {
        if (frame_number >= 0) {
            gui->draw();
            main_window->swap_buffers();
        }
    });

    auto camera = bnb::create_camera_device([input](bnb::full_image_t image) {
        input->push(image);
    }, 0);
    
//    gui->add_player_control(player);
    gui->add_output_control(window_output, "Screen output");

    main_window->set_glfw_events_callback([window_output, gui, player](const bnb::example::glfw_event& e) {
        if (e.type == bnb::example::glfw_event_t::framebuffer_resize) {
            window_output->set_window_size(e.size_width, e.size_height);
        } else if (e.type == bnb::example::glfw_event_t::key_press && e.keyboard_key == GLFW_KEY_F1) {
            gui->switch_show_hide_gui();
        }
        gui->on_glfw_event(e);
    }); 

    main_window->show_window_and_run_events_loop();

    return 0;
}
