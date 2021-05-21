#include "offscreen_effect_player.hpp"
#include "offscreen_render_target.hpp"

#include "glfw_window.hpp"
#include "render_thread.hpp"
#include "renderer.hpp"

#include <bnb/spal/camera/ocv_based.hpp>

#define BNB_CLIENT_TOKEN <#Place your token here#>

int main()
{
    // Frame size
    constexpr int32_t oep_width = 1280;
    constexpr int32_t oep_height = 720;

    std::shared_ptr<glfw_window> window = nullptr; // Should be declared here to destroy in the last turn

    // Init glfw for glfw_window 
    glfwInit();

    // Create an instance of our offscreen_render_target implementation, you can use your own.
    // pass dimension of processing frame
    auto ort = std::make_shared<bnb::offscreen_render_target>(oep_width, oep_height);

    // Create instance of offscreen effect player, pass path to location of effects,
    // token, dimension of processing frame (for best performance it is better to coincide
    // with camera frame dimensions), manual sound (useful fro some cases when sound 
    // should start and specified moment
    auto oep = bnb::interfaces::offscreen_effect_player::create({ BNB_RESOURCES_FOLDER }, BNB_CLIENT_TOKEN,
                                               oep_width, oep_height, false, ort);

    // Make glfw_window and render_thread only for show result of OEP
    // We want to share resources between context, we know that offscreen_render_target is based on GLFW and returned context
    // is GLFWwindow
    window = std::make_shared<glfw_window>("OEP Example", reinterpret_cast<GLFWwindow*>(ort->get_sharing_context()));
    std::shared_ptr<bnb::render::render_thread> render_t = std::make_shared<bnb::render::render_thread>(window->get_window(), oep_width, oep_height);
    auto key_func = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    };
    glfwSetKeyCallback(window->get_window(), key_func);

    oep->load_effect("effects/Afro");

    // Create and run instance of camera, pass callback for frames
    // Callback for received frame from the camera
    auto ef_cb = [&oep, render_t](bnb::full_image_t image) {
        auto image_ptr = std::make_shared<bnb::full_image_t>(std::move(image));

        // Callback for received pixel buffer from the offscreen effect player
        auto get_pixel_buffer_callback = [image_ptr, render_t](std::optional<ipb_sptr> pb) {
            if (pb.has_value()) {
                // Callback for update data in render thread
                auto render_callback = [render_t](std::optional<int> texture_id) {
                    if (texture_id.has_value()) {
                        render_t->update_data(*texture_id);
                    }
                };
                // Get texture id from shared context and render it
                (*pb)->get_texture(render_callback);
            }
        };

        std::optional<bnb::interfaces::orient_format> target_orient{ { bnb::camera_orientation::deg_0, true } };
        oep->process_image_async(image_ptr, get_pixel_buffer_callback, target_orient);
    };
    std::shared_ptr<bnb::camera_base> m_camera_ptr = bnb::create_camera_device(ef_cb, 0);

    window->show(oep_width, oep_height);
    window->run_main_loop();

    return 0;
}
