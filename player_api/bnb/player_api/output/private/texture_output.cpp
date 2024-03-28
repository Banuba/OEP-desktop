#include <bnb/player_api/output/texture_output.hpp>

#include <atomic>

namespace
{

    using namespace bnb::player_api;

    class texture_output_impl
        : public texture_output
    {
    public:
        texture_output_impl(const texture_ready_callback& callback)
            : m_callback(callback)
        {
        }

        ~texture_output_impl() = default;

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
            m_callback(render_target->get_output_texture());
        }

    private:
        texture_ready_callback m_callback;
        std::atomic_bool m_active{true};
    }; // class texture_output_impl

} // namespace

using t = bnb::player_api::texture_output;
std::shared_ptr<t> t::create(const t::texture_ready_callback& callback)
{
    return std::make_shared<texture_output_impl>(callback);
}
