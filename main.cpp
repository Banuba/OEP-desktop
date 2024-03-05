#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include "CoreFoundation/CoreFoundation.h"
#endif

#include <bnb/effect_player/utility.hpp>

#include <bnb/player_api/player/player.hpp>
#include <bnb/player_api/input/stream_input.hpp>
#include <bnb/player_api/output/window_output.hpp>

#include <bnb/spal/camera/base.hpp>
#include "glfw_window.hpp"
#include "opengl_context.hpp"

#include <iostream>

#define BNB_CLIENT_TOKEN <#PUT YOUR TOKEN HERE#>

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
    auto main_window = std::make_shared<bnb::glfw_window>("Player API Example"); // Should be declared here to destroy in the last turn

    // The usage of this class is necessary in order to properly initialize and deinitialize Banuba SDK
    bnb::utility utility(get_resources_folders(), BNB_CLIENT_TOKEN);
    bnb::utility::load_gl_functions();

    auto context = std::make_shared<bnb::opengl_context>(main_window);
    auto player = std::make_shared<bnb::player_api::player>(context);
    auto input = std::make_shared<bnb::player_api::stream_input>();
    auto output = std::make_shared<bnb::player_api::window_output>();

    player->use(input, output);
    player->play();
    player->load("effects/DebugFRX");

    auto camera_callback = [input](bnb::full_image_t image) {
        input->push(image);
    };
    auto camera = bnb::create_camera_device(camera_callback, 0);

    main_window->set_surface_changed_callback([output](int w, int h) {
        output->set_window_size(w, h);
    });

    main_window->show_and_run_events_loop();

    return 0;
}
