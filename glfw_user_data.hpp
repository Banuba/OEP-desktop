#pragma once

#include <bnb/effect_player/interfaces/effect_player.hpp>
#include <bnb/spal/camera/base.hpp>

#include <string>
#include <memory>

namespace bnb
{
    class glfw_user_data
    {
    public:
        glfw_user_data(
            offscreen_effect_player_sptr oep,
            render_t_sptr render_target,
            bnb::camera_sptr& camera,
            bnb::camera_base::push_frame_cb_t push_frame_cb)
            : m_oep(oep)
            , m_camera(camera)
            , m_render_target(render_target)
            , m_push_frame_cb(push_frame_cb)
        {
        }

        ~glfw_user_data()
        {
        }

        offscreen_effect_player_sptr oep()
        {
            return m_oep.lock();
        }

        render_t_sptr render_target()
        {
            return m_render_target.lock();
        }

        bnb::camera_sptr& camera_ptr()
        {
            return m_camera;
        }

        bnb::camera_base::push_frame_cb_t push_frame_cb()
        {
            return m_push_frame_cb;
        }
    private:
        std::weak_ptr<offscreen_effect_player_sptr::element_type> m_oep;
        bnb::camera_sptr& m_camera;
        std::weak_ptr<render_t_sptr::element_type> m_render_target;
        bnb::camera_base::push_frame_cb_t m_push_frame_cb;
    };
} // namespace viewer
