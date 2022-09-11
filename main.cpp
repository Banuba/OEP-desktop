#include <interfaces/offscreen_effect_player.hpp>

#include "render_context.hpp"
#include "effect_player.hpp"
#include "camera_utils.hpp"
#include "glfw_user_data.hpp"

#include <bnb/effect_player/utility.hpp>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include "CoreFoundation/CoreFoundation.h"
#endif

#define BNB_CLIENT_TOKEN <#Place your token here#>

int main()
{
    // Frame size
    constexpr int32_t oep_width = 1280;
    constexpr int32_t oep_height = 720;

    std::shared_ptr<glfw_window> window = nullptr; // Should be declared here to destroy in the last turn
                                               
    // Create an instance of effect_player implementation with cpp api, pass path to location of
    // effects and client token
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

    // The usage of this class is necessary in order to properly initialize and deinitialize Banuba SDK
    bnb::utility m_utility(dirs, BNB_CLIENT_TOKEN);

    // Create instance of render_context.
    // NOTE: each instance of Offscreen Render Target should have its own instance of Render Context
    auto rc = bnb::oep::interfaces::render_context::create();

    // Create an instance of our offscreen_render_target implementation, you can use your own.
    // NOTE: each instance of OEP should have its own instance of Offscreen Render Target
    // pass render_context
    auto ort = bnb::oep::interfaces::offscreen_render_target::create(rc);

    // Create our implementation of effect_player, pass effect player frame buffer sizes
    auto ep = bnb::oep::interfaces::effect_player::create(oep_width, oep_height);

    // Create instance of offscreen_effect_player, pass effect_player, offscreen_render_target
    // and dimensions of the processing frame (for the best performance it is better that they will coincide
    // with camera frame dimensions)
    auto oep = bnb::oep::interfaces::offscreen_effect_player::create(ep, ort, oep_width, oep_height);

    // Make glfw_window and render_thread only for show result of OEP
    // We want to share resources between context, we know that render_context is based on
    // GLFW and returned context is GLFWwindow
    window = std::make_shared<glfw_window>("OEP Example", reinterpret_cast<GLFWwindow*>(rc->get_sharing_context()));
    render_t_sptr render_t = std::make_shared<bnb::render::render_thread>(window->get_window(), oep_width, oep_height);

    oep->load_effect(<#Place the effect name here, e.g. effects/test_BG#>);

    // Callback for received frame from the camera
    auto camera_callback = [weak_oep = std::weak_ptr<decltype(oep)::element_type>(oep),
        weak_render_t = std::weak_ptr<decltype(render_t)::element_type>(render_t)](bnb::full_image_t image) {
        auto oep = weak_oep.lock();
        auto render_t = weak_render_t.lock();
        if (!oep || !render_t) {
            return;
        }
        // Callback for received pixel buffer from the offscreen effect player
        auto get_pixel_buffer_callback = [render_t](image_processing_result_sptr result) {
            if (result != nullptr) {
                // Callback for update data in render thread
                auto render_callback = [render_t](std::optional<rendered_texture_t> texture_id) {
                    if (texture_id.has_value()) {
                        auto gl_texture = static_cast<GLuint>(reinterpret_cast<int64_t>(*texture_id));
                        render_t->update_data(gl_texture);
                    }
                };
                // Get texture id from shared context and render it
                result->get_texture(render_callback);
            }
        };

        // Convert bnb full_image_t to OEP pixel_buffer
        // This function just wraps data from one type to another, without doing any manipulations with
        // the data itself, and without copying it
        auto pb_image = bnb::camera_utils::full_image_to_pixel_buffer(image);
        // Start image processing
        oep->process_image_async(pb_image, bnb::oep::interfaces::rotation::deg0, false, get_pixel_buffer_callback, bnb::oep::interfaces::rotation::deg0);
    };
    // Create and run instance of camera, pass callback for frames
    auto camera_ptr = bnb::create_camera_device(camera_callback, 0);

    bnb::glfw_user_data ud(oep, render_t, camera_ptr, camera_callback);

    glfwSetWindowUserPointer(window->get_window(), &ud);

    auto key_func = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto ud = static_cast<::bnb::glfw_user_data*>(glfwGetWindowUserPointer(window));
        if (!ud) {
            return;
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else if (key == GLFW_KEY_S) {
            if (auto oep = ud->oep()) {
                oep->stop();
                ud->camera_ptr().reset();
            }
        } else if (key == GLFW_KEY_P) {
            if (auto oep = ud->oep()) {
                if (ud->camera_ptr().get() == nullptr) {
                    ud->camera_ptr() = bnb::create_camera_device((ud->push_frame_cb(), 0);
                    oep->resume();
                }
            }
        }
    };
    glfwSetKeyCallback(window->get_window(), key_func);
    window->set_resize_callback([weak_window = std::weak_ptr<decltype(window)::element_type>(window)](int32_t w, int32_t h, int32_t w_glfw_buffer, int32_t h_glfw_buffer) {
        auto window = weak_window.lock();
        if (!window) {
            return;
        }
        auto ud = static_cast<::bnb::glfw_user_data*>(glfwGetWindowUserPointer(window->get_window()));
        if (!ud) {
            return;
        }

        // When minimizing a window on windows, glfw passes zero dimensions. Zero dimensions cannot be passed in OEP
        if (w <= 0 || h <= 0 || w_glfw_buffer <= 0 || h_glfw_buffer <= 0) {
            return;
        }
        if (auto render_t = ud->render_target(); render_t.get()) {
            render_t->surface_changed(w, h);
        }
        if (auto oep = ud->oep(); oep.get()) {
            oep->surface_changed(w, h);
        }
    });
    window->show(oep_width, oep_height);
    window->run_main_loop();

    return 0;
}
