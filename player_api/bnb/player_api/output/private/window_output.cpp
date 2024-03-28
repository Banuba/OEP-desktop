#include <bnb/player_api/output/window_output.hpp>
#include <bnb/player_api/output/private/orientable_scalable_base.hpp>

#include <atomic>
#include <cmath>

namespace
{
    
    using namespace bnb::player_api;

    class window_output_impl
        : public window_output
        , public orientable_scalable_base
    {
    public:
        window_output_impl() = default;

        ~window_output_impl() = default;

        void set_content_mode(content_mode new_content_mode) override
        {
            update_content_mode(new_content_mode);
        }
        
        void set_orientation(orientation orient, bool mirroring = false) override
        {
            update_orientation(orient, mirroring);
        }

        void active() override
        {
            m_active = true;
        }

        void deactive() override
        {
            m_active = false;
        }

        [[nodiscard]] bool is_active() override
        {
            return m_active;
        }

        void attach() override
        {
        }

        void detach() override
        {
        }

        void present(const output_sptr&, const render_target_sptr& render_target) override
        {
            int32_t x, y, w, h;
            oriented_frame_layout(render_target, m_frame_width, m_frame_height, x, y, w, h);
            render_target->prepare_to_screen_render();
            render_target->present(x + m_frame_left, y + m_frame_top, w, h, get_orientation_matrix());
        }

        void set_frame_layout(int32_t left, int32_t top, int32_t width, int32_t height) override
        {
            m_frame_left = left;
            m_frame_top = top;
            m_frame_width = width;
            m_frame_height = height;
        }

    protected:
        int32_t m_frame_left{0};
        int32_t m_frame_top{0};
        int32_t m_frame_width{0};
        int32_t m_frame_height{0};
        std::atomic_bool m_active{true};
    }; // class window_output_impl

} // namespace

std::shared_ptr<bnb::player_api::window_output> bnb::player_api::window_output::create()
{
    return std::make_shared<window_output_impl>();
}
