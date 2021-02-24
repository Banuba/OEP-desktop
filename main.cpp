#include "offscreen_effect_player.hpp"
#include "offscreen_render_target.hpp"

#include "glfw_window.hpp"
#include "render_thread.hpp"

#include <bnb/spal/camera/ocv_based.hpp>

#define BNB_CLIENT_TOKEN <#Place your token here#>

int main()
{
    // Frame size
    int32_t width = 1280;
    int32_t height = 720;

    // Init glfw for glfw_window 
    glfwInit();

    // Create an instance of our offscreen_render_target implementation, you can use your own.
    // pass dimension of processing frame
    std::optional<iort_sptr> ort = std::make_shared<bnb::offscreen_render_target>(width, height);

    // Create instance of offscreen effect player, pass path to location of effects,
    // token, dimension of processing frame (for best performance it is better to coincide
    // with camera frame dimensions), manual sound (useful fro some cases when sound 
    // should start and specified moment
    auto oep = bnb::offscreen_effect_player::create({ BNB_RESOURCES_FOLDER }, BNB_CLIENT_TOKEN,
                                               width, height, false, ort);
    oep->load_effect("effects/Afro");

    // Make glfw_window and render_thread only for show result of OEP
    std::shared_ptr<glfw_window> window = std::make_shared<glfw_window>("");
    std::shared_ptr<bnb::render::render_thread> render_t =
            std::make_shared<bnb::render::render_thread>(window->get_window(), width, height);

    // Callback for received frame from the camera
    auto ef_cb = [&oep, render_t](bnb::full_image_t image) {
        auto image_ptr = std::make_shared<bnb::full_image_t>(std::move(image));

        // Callback for received pixel buffer from the offscreen effect player
        auto get_pixel_buffer_callback = [image_ptr, render_t](std::optional<pb_sptr> pb) {
            if (pb.has_value()) {
                // Callback for update data in render thread
                auto render_callback = [render_t](std::optional<bnb::full_image_t> image) {
                    if (image.has_value()) {
                        render_t->update_data(std::move((*image)));
                    }
                };
                // Get the full_image_t contain nv12 from the offscreen render target
                (*pb)->get_nv12(render_callback);
            }
        };

        std::optional<bnb::interfaces::orient_format> target_orient{ { bnb::camera_orientation::deg_0, true } };
        oep->process_image_async(image_ptr, get_pixel_buffer_callback, target_orient);
    };

    // Create and run instance of camera, pass callback for frames
    std::shared_ptr<bnb::camera_base> m_camera_ptr = bnb::create_camera_device(ef_cb, 0);

    window->show(width, height);
    window->run_main_loop();

    return 0;
}
